const express = require("express");
const cors = require("cors");
const { spawn } = require("child_process");
const path = require("path");

const app = express();

// Allow frontend (React) to access backend
app.use(cors());

// Parse JSON bodies from frontend
app.use(express.json());

// Simple health-check
app.get("/", (req, res) => {
    res.send("Backend is running!");
});

// MAIN ROUTE - call C++ optimizer
app.post("/api/route", (req, res) => {

    // Path to your C++ executable
    const exePath = path.join(__dirname, "optimizer.exe");

    // Spawn the C++ program
    const child = spawn(exePath);

    // Send JSON input to C++ (stdin)
    child.stdin.write(JSON.stringify(req.body));
    child.stdin.end();

    let output = "";
    let errorOutput = "";

    // Collect stdout from C++
    child.stdout.on("data", (data) => {
        output += data.toString();
    });

    // Collect errors
    child.stderr.on("data", (data) => {
        errorOutput += data.toString();
    });

    // On finish
    child.on("close", (code) => {
        if (errorOutput.length > 0) {
            return res.status(500).send({ error: errorOutput });
        }

        try {
            const jsonData = JSON.parse(output);
            res.send(jsonData);
        } catch (err) {
            res.status(500).send({
                error: "Invalid JSON from C++",
                raw: output
            });
        }
    });
});

// Start the backend server
const PORT = 5000;
app.listen(PORT, () => {
    console.log(`Backend running on http://localhost:${PORT}`);
});