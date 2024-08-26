package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
	"time"

	"github.com/tarm/serial"
)

const width, height = 180, 180

func main() {
	config := &serial.Config{
		Name: "/dev/ttyUSB0",
		Baud: 115200,
	}

	port, err := serial.OpenPort(config)
	if err != nil {
		fmt.Printf("Failed to open port: %v\n", err)
		return
	}
	defer port.Close()

	outputFile, err := os.Create("rgbmap.csv")
	if err != nil {
		fmt.Printf("Failed to create output file: %v\n", err)
		return
	}
	defer outputFile.Close()

	fmt.Println("Starting to read data...")

	reader := bufio.NewReader(port)
	writer := bufio.NewWriter(outputFile)

	lineCount := 0
	for lineCount < width {
		port.Flush()
		line, err := reader.ReadString('\n')
		if err != nil {
			fmt.Printf("Error reading line: %v. Retrying...\n", err)
			time.Sleep(500 * time.Millisecond)
			continue
		}

		// Trim any trailing \r
		line = strings.TrimSuffix(line, "\r")

		_, err = writer.WriteString(line)
		if err != nil {
			fmt.Printf("Failed to write to output file: %v\n", err)
			break
		}

		lineCount++
		if lineCount%10 == 0 {
			fmt.Printf("Read %d lines so far...\n", lineCount)
		}
	}

	writer.Flush()
	fmt.Println("Finished writing 200 lines to output.txt")
}
