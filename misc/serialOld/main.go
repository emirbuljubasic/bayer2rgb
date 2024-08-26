package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"

	"github.com/tarm/serial"
)

// Constants for easy modification
const (
	serialPath = "/dev/ttyUSB0"
	baudRate   = 115200
	outputFile = "rgbmap.csv"
)

func main() {
	if len(os.Args) < 2 {
		log.Fatalf("Usage: %s <input.txt>\n", os.Args[0])
	}

	inputFile := os.Args[1]

	// Open the serial port
	config := &serial.Config{Name: serialPath, Baud: baudRate}
	port, err := serial.OpenPort(config)
	if err != nil {
		log.Fatalf("Failed to open serial port: %v", err)
	}
	defer port.Close()

	// Open the input file
	file, err := os.Open(inputFile)
	if err != nil {
		log.Fatalf("Failed to open input file: %v", err)
	}
	defer file.Close()

	// Open the output file for received data
	output, err := os.Create(outputFile)
	if err != nil {
		log.Fatalf("Failed to create output file: %v", err)
	}
	defer output.Close()

	scanner := bufio.NewScanner(file)
	lineCount := 0

	// Send the first three lines immediately, character by character
	for lineCount < 3 && scanner.Scan() {
		line := scanner.Text()
		sendNumbers(line, port)
		lineCount++
	}

	// Alternate between sending and receiving
	for scanner.Scan() {
		// Wait for the response from the serial port
		response, err := bufio.NewReader(port).ReadString('\n')
		if err != nil {
			log.Fatalf("Failed to read from serial port: %v", err)
		}

		// Save the received line to the output file
		if _, err := output.WriteString(response); err != nil {
			log.Fatalf("Failed to write to output file: %v", err)
		}

		// Send the next line from the input file, number by number
		line := scanner.Text()
		sendNumbers(line, port)
	}

	if err := scanner.Err(); err != nil {
		log.Fatalf("Error reading input file: %v", err)
	}

	fmt.Println("File transfer complete.")
}

// sendNumbers parses a line of text, extracts numbers separated by commas,
// and sends them one by one to the serial port
func sendNumbers(line string, port *serial.Port) {
	// Split the line by commas
	numbers := strings.Split(line, ",")
	for _, numberStr := range numbers {
		// Trim any whitespace and convert to an integer
		numberStr = strings.TrimSpace(numberStr)
		if number, err := strconv.Atoi(numberStr); err == nil {
			// Convert the integer to a byte and send it
			_, err := port.Write([]byte{byte(number)})
			if err != nil {
				log.Fatalf("Failed to write to serial port: %v", err)
			}
			// time.Sleep(10 * time.Millisecond) // Small delay between characters
		}
	}
}
