/*
FLAC��ǩͼƬ��ȡ�� Ver 1.0
��FLAC�ļ����ȶ�����ݡ���Ч����ݵ���ȡ��ͼƬ����
֧��BMP��JPEG��PNG��GIFͼƬ��ʽ
�ɽ�ͼƬ������ȡ���ļ����ڴ��У����ܰ�ȫ���ͷ��ڴ�
ʹ�÷�ʽ��ID3v2�汾��ͬ
ShadowPower ��2014/8/1 ҹ��
*/

#ifndef _ShadowPower_FLACPIC___
#define _ShadowPower_FLACPIC___
#define _CRT_SECURE_NO_WARNINGS
#ifndef NULL
#define NULL 0
#endif
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <cstring>

typedef unsigned char byte;
using namespace std;

namespace spFLAC {
	//FlacԪ���ݿ�ͷ���ṹ�嶨��
	struct FlacMetadataBlockHeader
	{
		byte flag;		//��־λ����1λ���Ƿ�Ϊ���һ�����ݿ飬��7λ�����ݿ�����
		byte length[3];	//���ݿ鳤�ȣ��������ݿ�ͷ��
	};

	byte *pPicData = 0;		//ָ��ͼƬ���ݵ�ָ��
	int picLength = 0;		//���ͼƬ���ݳ���
	char picFormat[4] = {};	//���ͼƬ���ݵĸ�ʽ����չ����

	//���ͼƬ��ʽ������1�����ݣ�����ֵ���Ƿ�ɹ�������ͼƬ��ʧ�ܣ�
	bool verificationPictureFormat(char *data)
	{
		//֧�ָ�ʽ��JPEG/PNG/BMP/GIF
		byte jpeg[2] = { 0xff, 0xd8 };
		byte png[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
		byte gif[6] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };
		byte gif2[6] = { 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
		byte bmp[2] = { 0x42, 0x4d };
		memset(&picFormat, 0, 4);
		if (memcmp(data, &jpeg, 2) == 0)
		{
			strcpy(picFormat, "jpg");
		}
		else if (memcmp(data, &png, 8) == 0)
		{
			strcpy(picFormat, "png");
		}
		else if (memcmp(data, &gif, 6) == 0 || memcmp(data, &gif2, 6) == 0)
		{
			strcpy(picFormat, "gif");
		}
		else if (memcmp(data, &bmp, 2) == 0)
		{
			strcpy(picFormat, "bmp");
		}
		else
		{
			return false;
		}

		return true;
	}

	//��ȫ�ͷ��ڴ�
	void freePictureData()
	{
		if (pPicData)
		{
			delete pPicData;
		}
		pPicData = 0;
		picLength = 0;
		memset(&picFormat, 0, 4);
	}

	//��ͼƬ��ȡ���ڴ棬����1���ļ�·�����ɹ�����true
	bool loadPictureData(const char *inFilePath)
	{
		freePictureData();
		FILE *fp = NULL;
		fp = fopen(inFilePath, "rb");
		if (!fp)						//�����ʧ��
		{
			fp = NULL;
			return false;
		}
		fseek(fp, 0, SEEK_SET);			//���ļ���ָ�뵽�ļ�ͷ��
		byte magic[4] = {};				//���У������
		memset(&magic, 0, 4);
		fread(&magic, 4, 1, fp);			//����У������
		byte fLaC[4] = { 0x66, 0x4c, 0x61, 0x43 };
		if (memcmp(&magic, &fLaC, 4) == 0)
		{
			//����У����ȷ���ļ�����ΪFlac
			FlacMetadataBlockHeader fmbh;	//����FlacԪ���ݿ�ͷ���ṹ��
			memset(&fmbh, 0, 4);			//����ڴ�
			fread(&fmbh, 4, 1, fp);			//����ͷ������
			//�������ݿ鳤�ȣ�����ͷ��
			int blockLength = fmbh.length[0] * 0x10000 + fmbh.length[1] * 0x100 + fmbh.length[2];
			int loopCount = 0;	//ѭ������������
			while ((fmbh.flag & 0x7f) != 6)
			{
				//����������Ͳ���ͼƬ���˴�ѭ��ִ��
				loopCount++;
				if (loopCount > 40)
				{
					//ѭ��40��û������ĩβ��ֱ��ֹͣ
					fclose(fp);
					fp = NULL;
					return false;					//�����ļ�������
				}
				fseek(fp, blockLength, SEEK_CUR);	//�������ݿ�
				if ((fmbh.flag & 0x80) == 0x80)
				{
					//�Ѿ������һ�����ݿ��ˣ���Ȼ����ͼƬ
					fclose(fp);
					fp = NULL;
					return false;					//û���ҵ�ͼƬ����
				}
				//ȡ����һ���ݿ�ͷ��
				memset(&fmbh, 0, 4);				//����ڴ�
				fread(&fmbh, 4, 1, fp);				//����ͷ������
				blockLength = fmbh.length[0] * 0x10000 + fmbh.length[1] * 0x100 + fmbh.length[2];//�������ݿ鳤��
			}
			//��ʱ�ѵ�ͼƬ���ݿ�

			int nonPicDataLength = 0;				//��ͼƬ���ݳ���
			fseek(fp, 4, SEEK_CUR);					//����֮Ծ
			nonPicDataLength += 4;
			char nextJumpLength[4];					//�´�Ҫ���ĳ���
			fread(&nextJumpLength, 4, 1, fp);		//��ȡ��ȫ��Ծ����
			nonPicDataLength += 4;
			int jumpLength = nextJumpLength[0] * 0x1000000 + nextJumpLength[1] * 0x10000 + nextJumpLength[2] * 0x100 + nextJumpLength[3];//�������ݿ鳤��
			fseek(fp, jumpLength, SEEK_CUR);		//Let's Jump!!
			nonPicDataLength += jumpLength;
			fread(&nextJumpLength, 4, 1, fp);
			nonPicDataLength += 4;
			jumpLength = nextJumpLength[0] * 0x1000000 + nextJumpLength[1] * 0x10000 + nextJumpLength[2] * 0x100 + nextJumpLength[3];
			fseek(fp, jumpLength, SEEK_CUR);		//Let's Jump too!!
			nonPicDataLength += jumpLength;
			fseek(fp, 20, SEEK_CUR);				//����֮Ծ
			nonPicDataLength += 20;

			//������������+����ļ���ʽ
			char tempData[20] = {};
			memset(tempData, 0, 20);
			fread(&tempData, 8, 1, fp);
			fseek(fp, -8, SEEK_CUR);	//�ص�ԭλ
			//�ж�40�Σ�һλһλ�����ļ�ͷ
			bool ok = false;			//�Ƿ���ȷʶ����ļ�ͷ
			for (int i = 0; i < 40; i++)
			{
				//У���ļ�ͷ
				if (verificationPictureFormat(tempData))
				{
					ok = true;
					break;
				}
				else
				{
					//���У��ʧ�ܳ��Լ������У��
					fseek(fp, 1, SEEK_CUR);
					nonPicDataLength++;
					fread(&tempData, 8, 1, fp);
					fseek(fp, -8, SEEK_CUR);
				}
			}

			if (!ok)
			{
				fclose(fp);
				fp = NULL;
				freePictureData();
				return false;			//�޷�ʶ�������
			}

			//-----�ִ�ͼƬ������-----
			picLength = blockLength - nonPicDataLength;		//����ͼƬ���ݳ���
			pPicData = new byte[picLength];					//��̬����ͼƬ�����ڴ�ռ�
			memset(pPicData, 0, picLength);					//���ͼƬ�����ڴ�
			fread(pPicData, picLength, 1, fp);				//�õ�ͼƬ����
			//------------------------
			fclose(fp);										//��������ɣ��ر��ļ���
		}
		else
		{
			//У��ʧ�ܣ�����Flac
			fclose(fp);
			fp = NULL;
			freePictureData();
			return false;
		}
		return true;
	}

	//ȡ��ͼƬ���ݵĳ���
	int getPictureLength()
	{
		return picLength;
	}

	//ȡ��ָ��ͼƬ���ݵ�ָ��
	byte *getPictureDataPtr()
	{
		return pPicData;
	}

	//ȡ��ͼƬ���ݵ���չ����ָ�룩
	char *getPictureFormat()
	{
		return picFormat;
	}

	bool writePictureDataToFile(const char *outFilePath)
	{
		FILE *fp = NULL;
		if (picLength > 0)
		{
			fp = fopen(outFilePath, "wb");		//��Ŀ���ļ�
			if (fp)								//�򿪳ɹ�
			{
				fwrite(pPicData, picLength, 1, fp);	//д���ļ�
				fclose(fp);							//�ر�
				return true;
			}
			else
			{
				return false;						//�ļ���ʧ��
			}
		}
		else
		{
			return false;						//û��ͼ������
		}
	}

	//��ȡͼƬ�ļ�������1�������ļ�������2������ļ�������ֵ���Ƿ�ɹ�
	bool extractPicture(const char *inFilePath, const char *outFilePath)
	{
		if (loadPictureData(inFilePath))	//���ȡ��ͼƬ���ݳɹ�
		{
			if (writePictureDataToFile(outFilePath))
			{
				return true;				//�ļ�д���ɹ�
			}
			else
			{
				return false;				//�ļ�д��ʧ��
			}
		}
		else
		{
			return false;					//��ͼƬ����
		}
		freePictureData();
	}
}
#endif