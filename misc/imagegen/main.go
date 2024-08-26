package main

import (
	"bufio"
	"image"
	"image/color"
	"image/png"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
)

const width, height = 64, 64

func main() {

	img := loadValues()

	// // Create a colored image of the given width and height.
	// img := image.NewNRGBA(image.Rect(0, 0, width, height))
	//
	// for y := 0; y < height; y++ {
	// 	for x := 0; x < width; x++ {
	// 		img.Set(x, y, color.NRGBA{
	// 			R: uint8((x + y) & 255),
	// 			G: uint8((x + y) << 1 & 255),
	// 			B: uint8((x + y) << 2 & 255),
	// 			A: 255,
	// 		})
	// 	}
	// }

	f, err := os.Create("image.png")
	if err != nil {
		log.Fatal(err)
	}

	if err := png.Encode(f, img); err != nil {
		f.Close()
		log.Fatal(err)
	}

	if err := f.Close(); err != nil {
		log.Fatal(err)
	}
}

func loadValues() *image.RGBA {
	file, err := os.Open("/home/emir/faks/diplomski/bayer2rgb/mcu/misc/imagegen/rgbmap.csv")
	if err != nil {
		panic(err)
	}
	img := image.NewRGBA(image.Rect(0, 0, width, height))
	rd := bufio.NewReader(file)
	i := 0
	for line, err := rd.ReadString('\n'); err != io.EOF; line, err = rd.ReadString('\n') {
		rgbs := strings.Split(line, ",")
		for j := 0; j < width; j++ {
			rgb := strings.Split(rgbs[j], ":")
			red, _ := strconv.Atoi(rgb[0])
			green, _ := strconv.Atoi(rgb[1])
			blue, _ := strconv.Atoi(rgb[2])
			img.Set(j, i, color.NRGBA{
				R: uint8(red),
				G: uint8(green),
				B: uint8(blue),
				A: 255,
			})
		}
		i++
	}

	return img
}
