package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"

	"go.bug.st/serial"
)

const (
	baudRate     = 115200         // Baud rate for UART communication
	serialPort   = "/dev/ttyUSB0" // Serial port to use for UART communication
	recvDataSize = 192            // Number of bytes expected from STM32F407G
)

func main() {
	if len(os.Args) != 2 {
		log.Fatalf("Usage: %s <input_file>\n", os.Args[0])
	}

	inputFileName := os.Args[1]

	// Open the serial port
	port, err := serial.Open(serialPort, &serial.Mode{BaudRate: baudRate})
	if err != nil {
		log.Fatalf("Error opening serial port: %v\n", err)
	}
	defer port.Close()

	log.Printf("Serial port %s opened successfully\n", serialPort)

	// Open the input file
	file, err := os.Open(inputFileName)
	if err != nil {
		log.Fatalf("Error opening input file: %v\n", err)
	}
	defer file.Close()

	// Prepare the output file
	outFile, err := os.Create("rgbmap.csv")
	if err != nil {
		log.Fatalf("Error creating output file: %v\n", err)
	}
	defer outFile.Close()

	// Read the file and send the first three rows
	scanner := bufio.NewScanner(file)
	rowCount := 0
	for scanner.Scan() {
		line := scanner.Text()
		numbers := strings.Split(line, ",")
		bytesToSend := make([]byte, len(numbers)-1) // -1 to ignore the trailing comma
		for i, numStr := range numbers[:len(numbers)-1] {
			num, _ := strconv.Atoi(numStr)
			bytesToSend[i] = byte(num)
		}

		// Send the data
		_, err := port.Write(bytesToSend)
		if err != nil {
			log.Fatalf("Error writing to serial port: %v\n", err)
		}
		log.Printf("Sent row %d: %s\n", rowCount+1, line)

		// If we've sent 3 rows, start the send/receive loop
		if rowCount >= 2 {
			break
		}
		rowCount++
	}

	// Receive data and send the remaining rows
	for scanner.Scan() {
		// time.Sleep(500 * time.Millisecond)
		// Receive 192 bytes from the STM32
		buf := make([]byte, 1)
		total := 0
		var val byte
		for val != 0xFF {
			n, err := port.Read(buf)
			if err != nil {
				log.Fatalf("Error reading from serial port: %v\n", err)
			}
			println("read n bytes:", n)
			val = buf[0]
			total += n
		}

		log.Printf("Received %d bytes\n", total)
		// Format the received data
		receivedStr := formatReceivedData(buf)

		// Write the formatted data to rgbmap.csv
		_, err = outFile.WriteString(receivedStr + "\n")
		if err != nil {
			log.Fatalf("Error writing to output file: %v\n", err)
		}

		// Send the next row
		line := scanner.Text()
		numbers := strings.Split(line, ",")
		bytesToSend := make([]byte, len(numbers)-1) // -1 to ignore the trailing comma
		for i, numStr := range numbers[:len(numbers)-1] {
			num, _ := strconv.Atoi(numStr)
			bytesToSend[i] = byte(num)
		}

		_, err = port.Write(bytesToSend)
		if err != nil {
			log.Fatalf("Error writing to serial port: %v\n", err)
		}
		log.Printf("Sent row %d: %s\n", rowCount+1, line)

		rowCount++
	}

	if err := scanner.Err(); err != nil {
		log.Fatalf("Error reading input file: %v\n", err)
	}

	log.Println("Finished communication")
}

// formatReceivedData formats the received byte slice into the required format
func formatReceivedData(data []byte) string {
	var result strings.Builder
	for i := 0; i < len(data); i += 3 {
		// if i > 0 {
		// 	result.WriteString(",")
		// }
		result.WriteString(fmt.Sprintf("%d:%d:%d,", data[i], data[i+1], data[i+2]))
	}
	return result.String()
}
