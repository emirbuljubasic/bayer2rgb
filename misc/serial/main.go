package main

import (
	"bufio"
	"io"
	"log"
	"os"
	"strings"

	"go.bug.st/serial"
)

const (
	portName       = "/dev/ttyUSB0"
	baudrate       = 115200
	outputFileName = "rgbmap.csv"
	recieveLength  = 192
	rows           = 64
)

func main() {
	if len(os.Args) != 2 {
		log.Fatalf("Usage: %s <input_file>\n", os.Args[0])
	}

	inputFileName := os.Args[1]

	mode := &serial.Mode{
		BaudRate: baudrate,
		DataBits: 8,
	}

	port, err := serial.Open(portName, mode)
	if err != nil {
		panic(err)
	}

	defer port.Close()

	log.Printf("Serial port %s opened successfully\n", portName)

	file, err := os.Open(inputFileName)
	if err != nil {
		panic(err)
	}

	defer file.Close()

	rd := bufio.NewReader(file)
	fn := func() (string, error) {
		line, err := rd.ReadString('\n')
		if err != nil {
			return "", err
		}

		return strings.TrimRight(line, "\n"), nil
	}

	var fileContents [][]byte
	for line, err := fn(); err != io.EOF; line, err = fn() {
		fileContents = append(fileContents, []byte(line))
	}

	fileContents = formatInputData(fileContents)

	i := 0
	for ; i < 3; i++ {
		n, err := port.Write([]byte(fileContents[i]))
		if err != nil {
			panic(err)
		}
		println("wrote bytes n:", n)
	}

	var outData [][]byte

	buf := make([]byte, 192)
	for ; i < rows; i++ {
		n, err := io.ReadFull(port, buf)
		if err != nil {
			panic(err)
		}

		println("recieved n bytes", n)

		if n < recieveLength {
			log.Fatal("full bytes not recieved, only recieved:", n)
		}

		outData = append(outData, buf)

		n, err = port.Write(fileContents[i])
		if err != nil {
			panic(err)
		}
		println("wrote bytes n:", n)
	}

	outputFile, err := os.Open(outputFileName)
	if err != nil {
		panic(err)
	}

	defer outputFile.Close()

	wr := bufio.NewWriter(outputFile)
	for i := 0; i < len(outData); i++ {
		wr.WriteString(string(fileContents[i]) + "\n")
	}
}

func formatInputData(fileContents [][]byte) [][]byte {
	var (
		data [][]byte
		line string
	)
	for _, val := range fileContents {
		line = string(val)
		strings.ReplaceAll(line, ":", "")
		data = append(data, []byte(line))
	}

	return data
}

func formatOutputData() {}
