#!/usr/bin/env python3
"""
Automated Test Runner for IIT Jodhpur Route Optimizer
Tests all functionality including Dijkstra and TSP algorithms
"""

import subprocess
import sys
import time
import re
from typing import List, Dict, Tuple, Optional
import os

class Color:
    """ANSI color codes for terminal output"""
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class TestCase:
    """Represents a single test case"""
    def __init__(self, name: str, choice: str, locations: List[str], 
                 expected_min_time: float, expected_max_time: float,
                 description: str = ""):
        self.name = name
        self.choice = choice  # "1" for Flexible, "2" for Fixed
        self.locations = locations
        self.expected_min_time = expected_min_time
        self.expected_max_time = expected_max_time
        self.description = description
        
    def get_input(self) -> str:
        """Generate input string for the test"""
        input_str = f"{self.choice}\n"
        input_str += f"{len(self.locations)}\n"
        input_str += "\n".join(self.locations) + "\n"
        input_str += "3\n"  # Exit
        return input_str

class TestResult:
    """Stores the result of a test execution"""
    def __init__(self, test_case: TestCase, passed: bool, actual_time: Optional[float],
                 actual_stops: Optional[int], error_msg: str = ""):
        self.test_case = test_case
        self.passed = passed
        self.actual_time = actual_time
        self.actual_stops = actual_stops
        self.error_msg = error_msg

class RouteOptimizerTester:
    """Main test runner class"""
    
    def __init__(self, executable_path: str = "./optimizer.exe"):
        self.executable = executable_path
        self.test_cases = []
        self.results = []
        self._load_test_cases()
        
    def _load_test_cases(self):
        """Load all test cases"""
        
        # Test Case 1: Small Fixed Order Path
        self.test_cases.append(TestCase(
            name="TC1: Small Fixed Order (Dijkstra)",
            choice="2",
            locations=["Main Gate", "Central Library", "Sports Complex"],
            expected_min_time=27.0,
            expected_max_time=29.0,
            description="Tests basic Dijkstra shortest path"
        ))
        
        # Test Case 2: Small TSP
        self.test_cases.append(TestCase(
            name="TC2: Small TSP (Flexible)",
            choice="1",
            locations=["Main Gate", "Cafeteria 1", "Central Library", "Sports Complex"],
            expected_min_time=31.0,
            expected_max_time=33.0,
            description="Tests TSP optimization with 4 locations"
        ))
        
        # Test Case 3: Adjacent Locations
        self.test_cases.append(TestCase(
            name="TC3: Adjacent Locations (Fixed)",
            choice="2",
            locations=["Sports Complex", "Gymnasium", "Student Activity Center"],
            expected_min_time=5.0,
            expected_max_time=5.0,
            description="Tests direct edge connections"
        ))
        
        # Test Case 4: Distant Locations TSP
        self.test_cases.append(TestCase(
            name="TC4: Distant Locations (Flexible)",
            choice="1",
            locations=["Hostel Complex A", "Research Park", "Garden Area", 
                      "Innovation Hub", "Medical Center"],
            expected_min_time=29.0,
            expected_max_time=31.0,
            description="Tests TSP across campus extremes"
        ))
        
        # Test Case 5: Star Pattern
        self.test_cases.append(TestCase(
            name="TC5: Star Pattern (Fixed)",
            choice="2",
            locations=["Central Plaza", "Main Gate", "Central Plaza", 
                      "Medical Center", "Central Plaza"],
            expected_min_time=14.0,
            expected_max_time=14.0,
            description="Tests returning to central hub"
        ))
        
        # Test Case 6: Medium TSP (10 locations)
        self.test_cases.append(TestCase(
            name="TC6: Medium TSP - 10 Locations",
            choice="1",
            locations=["Main Gate", "Academic Block 1", "Academic Block 2", 
                      "Central Library", "Lecture Hall Complex", "Cafeteria 1",
                      "Sports Complex", "Medical Center", "Central Plaza", 
                      "Innovation Hub"],
            expected_min_time=44.0,
            expected_max_time=48.0,
            description="Tests TSP DP with 10 locations"
        ))
        
        # Test Case 7: Single Location
        self.test_cases.append(TestCase(
            name="TC7: Single Location (Edge Case)",
            choice="1",
            locations=["Main Gate"],
            expected_min_time=0.0,
            expected_max_time=0.0,
            description="Tests edge case with single location"
        ))
        
        # Test Case 8: Two Adjacent Locations (Fixed)
        self.test_cases.append(TestCase(
            name="TC8: Two Locations Fixed",
            choice="2",
            locations=["Cafeteria 1", "Cafeteria 2"],
            expected_min_time=3.0,
            expected_max_time=3.0,
            description="Tests two adjacent locations with fixed order"
        ))
        
        # Test Case 9: Two Adjacent Locations (Flexible)
        self.test_cases.append(TestCase(
            name="TC9: Two Locations Flexible",
            choice="1",
            locations=["Cafeteria 1", "Cafeteria 2"],
            expected_min_time=3.0,
            expected_max_time=3.0,
            description="Tests two adjacent locations with flexible order"
        ))
        
        # Test Case 10: Circular Path
        self.test_cases.append(TestCase(
            name="TC10: Circular Path (Fixed)",
            choice="2",
            locations=["Main Gate", "Central Plaza", "Administrative Block",
                      "Medical Center", "Gymnasium", "Main Gate"],
            expected_min_time=27.0,
            expected_max_time=29.0,
            description="Tests complete circuit returning to start"
        ))
        
        # Test Case 11: Large TSP (tests greedy fallback)
        self.test_cases.append(TestCase(
            name="TC11: Large TSP - 16+ Locations (Greedy)",
            choice="1",
            locations=["Main Gate", "Academic Block 1", "Academic Block 2",
                      "Central Library", "Hostel Complex A", "Hostel Complex B",
                      "Sports Complex", "Gymnasium", "Student Activity Center",
                      "Cafeteria 1", "Cafeteria 2", "Mess Hall",
                      "Medical Center", "Auditorium", "Lecture Hall Complex",
                      "Research Park", "Central Plaza", "Innovation Hub"],
            expected_min_time=50.0,
            expected_max_time=100.0,
            description="Tests greedy TSP for large input (>15 nodes)"
        ))
        
        # Test Case 12: All Academic Buildings
        self.test_cases.append(TestCase(
            name="TC12: Academic Buildings Tour",
            choice="1",
            locations=["Academic Block 1", "Academic Block 2", "Central Library",
                      "Lecture Hall Complex", "Research Park", "Innovation Hub"],
            expected_min_time=15.0,
            expected_max_time=20.0,
            description="Tests academic buildings cluster"
        ))
        
        # Test Case 13: Food Places Tour
        self.test_cases.append(TestCase(
            name="TC13: Food Places Tour",
            choice="1",
            locations=["Cafeteria 1", "Cafeteria 2", "Mess Hall"],
            expected_min_time=5.0,
            expected_max_time=7.0,
            description="Tests food locations cluster"
        ))
        
    def run_test(self, test_case: TestCase) -> TestResult:
        """Run a single test case"""
        
        print(f"\n{Color.BOLD}{Color.OKBLUE}Running: {test_case.name}{Color.ENDC}")
        print(f"{Color.OKCYAN}{test_case.description}{Color.ENDC}")
        
        try:
            # Check if executable exists
            if not os.path.exists(self.executable):
                return TestResult(
                    test_case, False, None, None,
                    f"Executable not found: {self.executable}"
                )
            
            # Run the program
            start_time = time.time()
            process = subprocess.Popen(
                [self.executable],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            
            # Send input
            stdout, stderr = process.communicate(
                input=test_case.get_input(),
                timeout=30
            )
            
            execution_time = time.time() - start_time
            
            # Parse output
            total_time, stops = self._parse_output(stdout)
            
            # Validate results
            if total_time is None:
                return TestResult(
                    test_case, False, None, None,
                    "Could not parse total time from output"
                )
            
            # Check if time is within expected range
            time_valid = (test_case.expected_min_time <= total_time <= 
                         test_case.expected_max_time)
            
            # Check if number of stops is correct
            stops_valid = (stops == len(test_case.locations))
            
            passed = time_valid and stops_valid
            
            # Print results
            if passed:
                print(f"{Color.OKGREEN}✓ PASSED{Color.ENDC}")
            else:
                print(f"{Color.FAIL}✗ FAILED{Color.ENDC}")
            
            print(f"  Expected Time: {test_case.expected_min_time:.2f} - "
                  f"{test_case.expected_max_time:.2f} min")
            print(f"  Actual Time: {total_time:.2f} min")
            print(f"  Expected Stops: {len(test_case.locations)}")
            print(f"  Actual Stops: {stops if stops else 'N/A'}")
            print(f"  Execution Time: {execution_time:.2f}s")
            
            error_msg = ""
            if not time_valid:
                error_msg += f"Time out of range. "
            if not stops_valid:
                error_msg += f"Incorrect number of stops. "
            
            return TestResult(test_case, passed, total_time, stops, error_msg)
            
        except subprocess.TimeoutExpired:
            return TestResult(
                test_case, False, None, None,
                "Test timed out (>30s)"
            )
        except Exception as e:
            return TestResult(
                test_case, False, None, None,
                f"Exception: {str(e)}"
            )
    
    def _parse_output(self, output: str) -> Tuple[Optional[float], Optional[int]]:
        """Parse the program output to extract total time and stops"""
        
        total_time = None
        stops = None
        
        # Match "Total Time: XX.XX minutes"
        time_match = re.search(r'Total Time:\s*([0-9.]+)\s*minutes', output)
        if time_match:
            total_time = float(time_match.group(1))
        
        # Match "Stops: X locations"
        stops_match = re.search(r'Stops:\s*(\d+)\s*locations?', output)
        if stops_match:
            stops = int(stops_match.group(1))
        
        return total_time, stops
    
    def run_all_tests(self):
        """Run all test cases"""
        
        print(f"\n{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD}  IIT JODHPUR ROUTE OPTIMIZER - TEST SUITE{Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")
        print(f"\nTotal Test Cases: {len(self.test_cases)}\n")
        
        for test_case in self.test_cases:
            result = self.run_test(test_case)
            self.results.append(result)
        
        self._print_summary()
    
    def _print_summary(self):
        """Print test summary"""
        
        passed = sum(1 for r in self.results if r.passed)
        failed = len(self.results) - passed
        
        print(f"\n{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD}  TEST SUMMARY{Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}\n")
        
        print(f"Total Tests: {len(self.results)}")
        print(f"{Color.OKGREEN}Passed: {passed}{Color.ENDC}")
        print(f"{Color.FAIL}Failed: {failed}{Color.ENDC}")
        print(f"Success Rate: {(passed/len(self.results)*100):.1f}%\n")
        
        if failed > 0:
            print(f"{Color.WARNING}Failed Tests:{Color.ENDC}")
            for result in self.results:
                if not result.passed:
                    print(f"  - {result.test_case.name}")
                    if result.error_msg:
                        print(f"    {Color.FAIL}Error: {result.error_msg}{Color.ENDC}")
        
        print(f"\n{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}\n")
    
    def run_specific_test(self, test_number: int):
        """Run a specific test by number (1-indexed)"""
        
        if 1 <= test_number <= len(self.test_cases):
            test_case = self.test_cases[test_number - 1]
            result = self.run_test(test_case)
            self.results.append(result)
        else:
            print(f"{Color.FAIL}Invalid test number. Choose 1-{len(self.test_cases)}{Color.ENDC}")

def main():
    """Main entry point"""
    
    # Check for command line arguments
    if len(sys.argv) > 1:
        if sys.argv[1] == "--help" or sys.argv[1] == "-h":
            print("Usage:")
            print("  python test_runner.py              # Run all tests")
            print("  python test_runner.py <number>     # Run specific test")
            print("  python test_runner.py --list       # List all tests")
            return
        
        if sys.argv[1] == "--list":
            tester = RouteOptimizerTester()
            print("\nAvailable Test Cases:")
            for i, tc in enumerate(tester.test_cases, 1):
                print(f"{i:2d}. {tc.name}")
                print(f"    {tc.description}")
            return
        
        try:
            test_num = int(sys.argv[1])
            tester = RouteOptimizerTester()
            tester.run_specific_test(test_num)
            return
        except ValueError:
            print(f"{Color.FAIL}Invalid argument. Use --help for usage.{Color.ENDC}")
            return
    
    # Run all tests
    tester = RouteOptimizerTester()
    tester.run_all_tests()

if __name__ == "__main__":
    main()
