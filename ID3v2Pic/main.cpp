#include "ID3v2Pic.h"
#include "FlacPic.h"

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		if (!spID3::extractPicture(argv[1], argv[2]))
			spFLAC::extractPicture(argv[1], argv[2]);
	}
	else if (argc = 2)
	{
		if (argv[1] != NULL)
		{
			if (spID3::loadPictureData(argv[1]))
			{
				char outputFile[20] = "output.";
				strcat(outputFile, spID3::getPictureFormat());
				spID3::writePictureDataToFile(outputFile);
			}
			else if (spFLAC::loadPictureData(argv[1]))
			{
				char outputFile[20] = "output.";
				strcat(outputFile, spFLAC::getPictureFormat());
				spFLAC::writePictureDataToFile(outputFile);
			}
		}
	}
	return 0;
}