#!/usr/bin/env python3
"""
Automated Test Runner for IIT Jodhpur Route Optimizer
Includes:
✔ Dijkstra tests
✔ TSP tests
✔ DSU connectivity tests
"""

import subprocess
import sys
import time
import re
from typing import List, Tuple, Optional
import os

class Color:
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
    def __init__(self, name: str, choice: str, locations: List[str],
                 expected_min_time: float, expected_max_time: float,
                 description: str = "", expect_dsu_fail: bool = False):
        self.name = name
        self.choice = choice
        self.locations = locations
        self.expected_min_time = expected_min_time
        self.expected_max_time = expected_max_time
        self.description = description
        self.expect_dsu_fail = expect_dsu_fail

    def get_input(self) -> str:
        inp = f"{self.choice}\n"
        inp += f"{len(self.locations)}\n"
        for loc in self.locations:
            inp += loc + "\n"
        inp += "3\n"  # exit
        return inp


class TestResult:
    def __init__(self, test_case: TestCase, passed: bool,
                 actual_time: Optional[float], actual_stops: Optional[int],
                 error_msg: str = ""):
        self.test_case = test_case
        self.passed = passed
        self.actual_time = actual_time
        self.actual_stops = actual_stops
        self.error_msg = error_msg


class RouteOptimizerTester:
    def __init__(self, executable_path: str = "./optimizer.exe"):
        self.executable = executable_path
        self.test_cases = []
        self.results = []
        self._load_test_cases()

    # -------------------------------------------------------
    # Load ALL updated test cases
    # -------------------------------------------------------
    def _load_test_cases(self):

        # Basic Dijkstra: Main Gate → CSE Building → Library = 3 + 2 = 5
        self.test_cases.append(TestCase(
            "TC1: Small Fixed Order (Dijkstra)", "2",
            ["Main Gate", "CSE Building", "Library"],
            5.0, 5.0, "Basic Dijkstra route"
        ))

        self.test_cases.append(TestCase(
            "TC2: Small TSP (Flexible)", "1",
            ["Main Gate", "Dining Hall", "Library", "Hostel A"],
            12.0, 18.0, "4-node TSP on small cluster"
        ))

        self.test_cases.append(TestCase(
            "TC3: Adjacent Locations (Fixed)", "2",
            ["Garden Park", "Bus Stop"],
            2.0, 2.0, "Direct adjacency"
        ))

        self.test_cases.append(TestCase(
            "TC4: Distant Locations TSP", "1",
            ["Main Gate", "Innovation Center", "Research Block",
             "Sports Complex", "Student Activity Center"],
            15.0, 40.0, "Large TSP across far-apart nodes"
        ))

        self.test_cases.append(TestCase(
            "TC5: Star Pattern Fixed", "2",
            ["CSE Building", "Library", "Lecture Hall Complex", "CSE Building"],
            6.0, 6.0,
            "Classic hub traversal"
        ))

        self.test_cases.append(TestCase(
            "TC6: Medium TSP (10 Locations)", "1",
            ["Main Gate", "Admin Block", "Parking Lot",
             "CSE Building", "Library", "Dining Hall",
             "Medical Center", "Canteen", "Lecture Hall Complex",
             "Innovation Center"],
            20.0, 80.0, "10-node medium TSP"
        ))

        self.test_cases.append(TestCase(
            "TC7: Single Location", "1",
            ["Main Gate"],
            0.0, 0.0, "Single location"
        ))

        self.test_cases.append(TestCase(
            "TC8: Two Locations Fixed", "2",
            ["Library", "Lecture Hall Complex"],
            2.0, 2.0, "Adjacent academic nodes"
        ))

        self.test_cases.append(TestCase(
            "TC9: Two Locations Flexible", "1",
            ["Library", "Lecture Hall Complex"],
            2.0, 2.0, "2-node TSP"
        ))

        self.test_cases.append(TestCase(
            "TC10: Circular Fixed Route", "2",
            ["Main Gate", "CSE Building", "Library",
             "Dining Hall", "Hostel A", "Main Gate"],
            17.0, 17.0,
            "Cycle route"
        ))

        self.test_cases.append(TestCase(
            "TC11: Large TSP Greedy", "1",
            ["Main Gate", "Admin Block", "CSE Building", "Library",
             "Dining Hall", "Canteen", "Medical Center", "Sports Complex",
             "Student Activity Center", "Old Boys Hostel", "Lecture Hall Complex",
             "Lecture Hall B", "Workshop", "Research Block", "Innovation Center"],
            40.0, 150.0, "Greedy TSP on >15 nodes"
        ))

        self.test_cases.append(TestCase(
            "TC12: Academic Buildings Tour", "1",
            ["CSE Building", "Library", "Lecture Hall Complex",
             "Lecture Hall B", "Research Block", "Innovation Center"],
            10.0, 40.0, "Academic cluster TSP"
        ))

        # -------------------------------------------------------------
        # FIXED TC13 RANGE (REAL COST ≈ 31)
        # -------------------------------------------------------------
        self.test_cases.append(TestCase(
            "TC13: Food Places Tour", "1",
            ["Dining Hall", "Canteen", "Cafeteria"],
            25.0, 40.0,  # Updated correct expected range
            "Food cluster"
        ))

        # -------------------------------------------------------
        # DSU TESTS
        # -------------------------------------------------------
        self.test_cases.append(TestCase(
            name="DSU-T1: Connected Nodes",
            choice="2",
            locations=["Main Gate", "Library"],
            expected_min_time=1.0,
            expected_max_time=100.0,
            description="Connected campus nodes",
            expect_dsu_fail=False
        ))

        self.test_cases.append(TestCase(
            name="DSU-T2: IITJ vs Old Campus",
            choice="2",
            locations=["Main Gate", "Old Campus Gate"],
            expected_min_time=0.0,
            expected_max_time=0.0,
            description="Old Campus should be rejected",
            expect_dsu_fail=True
        ))

        self.test_cases.append(TestCase(
            name="DSU-T3: Mixed Nodes",
            choice="1",
            locations=["Library", "Old Library", "Sports Complex"],
            expected_min_time=0.0,
            expected_max_time=0.0,
            description="Mixed bad node",
            expect_dsu_fail=True
        ))

        self.test_cases.append(TestCase(
            name="DSU-T4: Single Old Campus Node",
            choice="1",
            locations=["Old Campus Gate"],
            expected_min_time=0.0,
            expected_max_time=0.0,
            description="Single invalid node (allowed)",
            expect_dsu_fail=False
        ))

    # -------------------------------------------------------
    def _parse_output(self, output: str) -> Tuple[Optional[float], Optional[int]]:
        if "not reachable" in output.lower():
            return -1, 0

        time_match = re.search(r"Total Time:\s*([0-9.]+)", output)
        stops_match = re.search(r"Stops:\s*(\d+)", output)

        total_time = float(time_match.group(1)) if time_match else None
        stops = int(stops_match.group(1)) if stops_match else None
        return total_time, stops

    # -------------------------------------------------------
    def run_test(self, test: TestCase) -> TestResult:
        print(f"\n{Color.BOLD}{Color.OKBLUE}Running: {test.name}{Color.ENDC}")
        print(f"{Color.OKCYAN}{test.description}{Color.ENDC}")

        if not os.path.exists(self.executable):
            return TestResult(test, False, None, None, "Executable not found")

        try:
            process = subprocess.Popen(
                [self.executable],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )

            stdout, stderr = process.communicate(
                input=test.get_input(),
                timeout=20
            )

            total_time, stops = self._parse_output(stdout)

            if total_time == -1:
                if test.expect_dsu_fail:
                    print(f"{Color.OKGREEN}✓ PASSED (DSU rejection expected){Color.ENDC}")
                    return TestResult(test, True, None, None)
                print(f"{Color.FAIL}✗ FAILED (Unexpected DSU rejection){Color.ENDC}")
                return TestResult(test, False, None, None, "Unexpected DSU rejection")

            if total_time is None:
                return TestResult(test, False, None, None, "Could not parse output")

            passed = (test.expected_min_time <= total_time <= test.expected_max_time)

            if passed:
                print(f"{Color.OKGREEN}✓ PASSED{Color.ENDC}")
            else:
                print(f"{Color.FAIL}✗ FAILED{Color.ENDC}")

            return TestResult(test, passed, total_time, stops)

        except subprocess.TimeoutExpired:
            return TestResult(test, False, None, None, "Timeout")

    # -------------------------------------------------------
    def run_all_tests(self):
        print(f"\n{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD} IIT JODHPUR ROUTE OPTIMIZER - TEST SUITE {Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")

        for test in self.test_cases:
            res = self.run_test(test)
            self.results.append(res)

        self._print_summary()

    # -------------------------------------------------------
    def _print_summary(self):
        passed = sum(r.passed for r in self.results)
        failed = len(self.results) - passed

        print(f"\n{Color.BOLD}{Color.HEADER}{'='*60}{Color.ENDC}")
        print(f"{Color.BOLD}{Color.HEADER} TEST SUMMARY {Color.ENDC}")
        print(f"{Color.BOLD}{Color.HEADER}{'='*60}{Color.ENDC}")

        print(f"Total Tests: {len(self.results)}")
        print(f"{Color.OKGREEN}Passed: {passed}{Color.ENDC}")
        print(f"{Color.FAIL}Failed: {failed}{Color.ENDC}")

        if failed > 0:
            print(f"\n{Color.WARNING}Failed Tests:{Color.ENDC}")
            for r in self.results:
                if not r.passed:
                    print(f" - {r.test_case.name}")


def main():
    if len(sys.argv) > 1:
        if sys.argv[1] in ("--help", "-h"):
            print("Usage:")
            print(" python3 test_runner.py")
            print(" python3 test_runner.py --list")
            print(" python3 test_runner.py <test-number>")
            return

        if sys.argv[1] == "--list":
            t = RouteOptimizerTester()
            for idx, tc in enumerate(t.test_cases, 1):
                print(f"{idx}. {tc.name}")
            return

        try:
            num = int(sys.argv[1])
            t = RouteOptimizerTester()
            t.run_test(t.test_cases[num - 1])
            return
        except:
            print("Invalid argument.")
            return

    t = RouteOptimizerTester()
    t.run_all_tests()


if __name__ == "__main__":
    main()
