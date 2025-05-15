input ?= "images/image.ppm"


naif: projet.c
	gcc -Wall -Wextra -std=c99 -DNAIF -Dinput_file=\"$(input)\" -o naif projet.c
	./naif
	del /f /q naif.exe 2>nul || exit 0

stats: projet.c
	gcc -Wall -Wextra -std=c99 -DSTATS -Dinput_file=\"$(input)\" -o stats projet.c
	./stats
	del /f /q stats.exe 2>nul || exit 0

huffman: projet.c
	gcc -Wall -Wextra -std=c99 -DCOMPRESS_HUFFMAN -Dinput_file=\"$(input)\" -o huffman projet.c
	./huffman
	del /f /q huffman.exe 2>nul || exit 0

decompress_huffman: projet.c 
	gcc -Wall -Wextra -std=c99 -DDECOMPRESS_HUFFMAN -o decompress projet.c
	./decompress
	del /f /q decompress.exe 2>nul || exit 0

clean:
	del /f /q image_naif.hppm 2>nul || exit 0
	del /f /q image_naif_reconstruit.ppm 2>nul || exit 0
	del /f /q image_huffman.hppm 2>nul || exit 0
	del /f /q image_huffman_reconstruit.ppm 2>nul || exit 0
	del /f /q histogram.ppm 2>nul || exit 0m