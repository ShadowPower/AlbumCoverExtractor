# AlbumCoverExtractor
Extract album cover picture from your media files, you can also use it for your music player.

It is a C++ Libary, can also be used as a terminal tool.

## API
Use the __spID3__ namespace to extract the album cover in MPEG, and use the __spFLAC__ namespace for FLAC.

Because the code is written like C, it can be easily changed to C language.

__This library will define an unsigned char type as a byte.__

```cpp
// First you need to include these files
#include "ID3v2Pic.h"
#include "FlacPic.h"

/*
Read the album cover picture data from the media file
Parameter 1: File path
Return: true if successful
*/
bool loadPictureData(const char *inFilePath);


/*
Get the picture data pointer
Return: Pointer to picture data starting point
*/
byte *getPictureDataPtr();


/*
Get the picture data size
Return: The number of bytes of image data
*/
int getPictureLength();


/*
Generate an image file extension
Return: Pointer to the character array that stores the file extension
*/
char *getPictureFormat();


/*
Write out the picture data to a file
Parameter 1: File path
Return: true if successful
*/
bool writePictureDataToFile(const char *outFilePath);


/*
Extract album cover from the media file
Parameter 1: Input media file path
Parameter 2: output image file path
Return: true if successful
*/
bool extractPicture(const char *inFilePath, const char *outFilePath);

/* free the memory */
void freePictureData();
```

## Terminal tool
Supports MPEG and FLAC media files.

Extract images from media file like this:

> ID3v2Pic media.mp3

You will get the picture file, name is __output.(extension)__ .

Of course, you can also specify the output file name:

> ID3v2Pic media.mp3 cover.jpg
