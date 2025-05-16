input ?= "images/image.ppm"


naif: projet.c
	gcc -Wall -Wextra -std=c99 -DNAIF -Dinput_file=\"$(input)\" -o naif projet.c
	./naif
	del /f /q naif.exe 2>nul || exit 0
# 	rm -f naif

stats: projet.c
	gcc -Wall -Wextra -std=c99 -DSTATS -Dinput_file=\"$(input)\" -o stats projet.c
	./stats
	del /f /q stats.exe 2>nul || exit 0
# 	rm -f stats

huffman: projet.c
	gcc -Wall -Wextra -std=c99 -DCOMPRESS_HUFFMAN -Dinput_file=\"$(input)\" -o huffman projet.c
	./huffman
	del /f /q huffman.exe 2>nul || exit 0
# 	rm -f huffman


decompress_huffman: projet.c 
	gcc -Wall -Wextra -std=c99 -DDECOMPRESS_HUFFMAN -o decompress projet.c
	./decompress
	del /f /q decompress.exe 2>nul || exit 0
#	rm -f decompress

clean:
	del /f /q images\image.ppm 2>nul || exit 0
# 	rm -f images/image.ppm
	del /f /q images\image_1x1.ppm 2>nul || exit 0
#	rm -f images/image_1x1.ppm
	del /f /q image_naif.hppm 2>nul || exit 0
#	rm -f image_naif.hppm
	del /f /q image_naif_reconstruit.ppm 2>nul || exit 0
#	rm -f image_naif_reconstruit.ppm
	del /f /q image_huffman.hppm 2>nul || exit 0
#	rm -f image_huffman.hppm
	del /f /q image_huffman_reconstruit.ppm 2>nul || exit 0
#	rm -f image_huffman_reconstruit.ppm
	del /f /q histogram.ppm 2>nul || exit 0m
#	rm -f histogram.ppm