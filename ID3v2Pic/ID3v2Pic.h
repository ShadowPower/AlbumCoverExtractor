/*
ID3v2��ǩͼƬ��ȡ�� Ver 1.0
֧��ID3v2���а汾
��ID3v2��ǩ���ȶ�����ݡ���Ч����ݵ���ȡ��ͼƬ����
֧��BMP��JPEG��PNG��GIFͼƬ��ʽ
�ɽ�ͼƬ������ȡ���ļ����ڴ��У����ܰ�ȫ���ͷ��ڴ�
ShadowPower ��2014/8/1 ����
*/

#ifndef _ShadowPower_ID3V2PIC___
#define _ShadowPower_ID3V2PIC___
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

namespace spID3 {
	//ID3v2��ǩͷ���ṹ�嶨��
	struct ID3V2Header
	{
		char  identi[3];//ID3ͷ��У�飬����Ϊ��ID3��������Ϊ������ID3��ǩ
		byte  major;	//ID3�汾�ţ�3��ID3v2.3��4��ID3v2.4���Դ�����
		byte  revsion;	//ID3���汾�ţ��˰汾Ϊ00
		byte  flags;    //��־λ
		byte  size[4];	//��ǩ��С��������ǩͷ��10���ֽ�
	};

	//ID3v2��ǩ֡ͷ���ṹ�嶨��
	struct ID3V2FrameHeader
	{
		char FrameId[4];//��ʶ�������������˱�ǩ֡����������
		byte size[4];	//��ǩ֡�Ĵ�С��������ǩͷ��10���ֽ�  
		byte flags[2];	//��־λ  
	};

	struct ID3V22FrameHeader
	{
		char FrameId[3];//��ʶ�������������˱�ǩ֡����������
		byte size[3];	//��ǩ֡�Ĵ�С��������ǩͷ��6���ֽ�  
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
		FILE *fp = NULL;				//��ʼ���ļ�ָ�룬�ÿ�
		fp = fopen(inFilePath, "rb");	//��ֻ��&�����Ʒ�ʽ���ļ�
		if (!fp)						//�����ʧ��
		{
			fp = NULL;
			return false;
		}
		fseek(fp, 0, SEEK_SET);			//���ļ���ָ�뵽�ļ�ͷ����ӡ���д�֮��Ĭ����β����

		//��ȡ
		ID3V2Header id3v2h;				//����һ��ID3v2��ǩͷ�ṹ��
		memset(&id3v2h, 0, 10);			//�ڴ���0��10���ֽ�
		fread(&id3v2h, 10, 1, fp);		//���ļ�ͷ��10���ֽ�д��ṹ���ڴ�

		//�ļ�ͷʶ��
		if (strncmp(id3v2h.identi, "ID3", 3) != 0)
		{
			fclose(fp);
			fp = NULL;
			return false;//û��ID3��ǩ
		}

		//�����е�����Ӧ���Ѿ��ɹ����ļ���

		//����������ǩ���ȣ�ÿ���ֽڽ�7λ��Ч
		int tagTotalLength = (id3v2h.size[0] & 0x7f) * 0x200000 + (id3v2h.size[1] & 0x7f) * 0x4000 + (id3v2h.size[2] & 0x7f) * 0x80 + (id3v2h.size[3] & 0x7f);

		if (id3v2h.major == 3 || id3v2h.major == 4)	//ID3v2.3 �� ID3v2.4
		{
			ID3V2FrameHeader id3v2fh;		//����һ��ID3v2��ǩ֡ͷ�ṹ��
			memset(&id3v2fh, 0, 10);

			bool hasExtendedHeader = ((id3v2h.flags >> 6 & 0x1) == 1);//�Ƿ�����չͷ

			if (hasExtendedHeader)
			{
				//�������չͷ
				byte extendedHeaderSize[4] = {};
				memset(&extendedHeaderSize, 0, 4);
				fread(&extendedHeaderSize, 4, 1, fp);
				//ȡ����չͷ��С�������������ݣ�
				int extendedHeaderLength = extendedHeaderSize[0] * 0x1000000 + extendedHeaderSize[1] * 0x10000 + extendedHeaderSize[2] * 0x100 + extendedHeaderSize[3];
				//������չͷ
				fseek(fp, extendedHeaderLength, SEEK_CUR);
			}

			fread(&id3v2fh, 10, 1, fp);				//������д��ID3V2FrameHeader�ṹ����
			int curDataLength = 10;					//��ŵ�ǰ�Ѿ���ȡ�����ݴ�С���ղ��Ѿ�����10�ֽ�
			while ((strncmp(id3v2fh.FrameId, "APIC", 4) != 0))//���֡ͷû��APIC��ʶ����ѭ��ִ��
			{
				if (curDataLength > tagTotalLength)
				{
					fclose(fp);
					fp = NULL;
					return false;					//δ����ͼƬ����
				}
				//����֡���ݳ���
				//ʹ��int�������������Լ2GB����ǩ֡û����ô��ɡ�����
				int frameLength = id3v2fh.size[0] * 0x1000000 + id3v2fh.size[1] * 0x10000 + id3v2fh.size[2] * 0x100 + id3v2fh.size[3];
				fseek(fp, frameLength, SEEK_CUR);	//��ǰ��Ծ����һ��֡ͷ
				memset(&id3v2fh, 0, 10);			//���֡ͷ�ṹ������
				fread(&id3v2fh, 10, 1, fp);			//���¶�ȡ����
				curDataLength += frameLength + 10;	//��¼��ǰ���ڵ�ID3��ǩλ�ã��Ա��˳�ѭ��
			}

			//����һ�µ�ǰͼƬ֡�����ݳ���
			int frameLength = id3v2fh.size[0] * 0x1000000 + id3v2fh.size[1] * 0x10000 + id3v2fh.size[2] * 0x100 + id3v2fh.size[3];

			/*
			����ID3v2.3ͼƬ֡�Ľṹ��

			<Header for 'Attached picture', ID: "APIC">
			ͷ��10���ֽڵ�֡ͷ

			Text encoding      $xx
			Ҫ����һ���ֽڣ����ֱ��룩

			MIME type          <text string> $00
			�������ı� + /0��������ɵõ��ļ���ʽ

			Picture type       $xx
			����һ���ֽڣ�ͼƬ���ͣ�

			Description        <text string according to encoding> $00 (00)
			�������ı� + /0��������ɵõ�������Ϣ

			Picture data       <binary data>
			����������ͼƬ����
			*/
			int nonPicDataLength = 0;	//��ͼƬ���ݵĳ���
			fseek(fp, 1, SEEK_CUR);		//����֮Ծ
			nonPicDataLength++;

			char tempData[20] = {};		//��ʱ������ݵĿռ�
			char mimeType[20] = {};		//ͼƬ����
			int mimeTypeLength = 0;		//ͼƬ�����ı�����

			fread(&tempData, 20, 1, fp);//ȡ��һС������
			fseek(fp, -20, SEEK_CUR);	//�ص�ԭλ

			strcpy(mimeType, tempData);				//���Ƴ�һ���ַ���
			mimeTypeLength = strlen(mimeType) + 1;	//�����ַ������ȣ�����ĩβ00
			fseek(fp, mimeTypeLength, SEEK_CUR);	//����������֮��
			nonPicDataLength += mimeTypeLength;		//��¼����

			fseek(fp, 1, SEEK_CUR);		//��һ������֮Ծ
			nonPicDataLength++;

			int temp = 0;				//��¼��ǰ�ֽ����ݵı���
			fread(&temp, 1, 1, fp);		//��ȡһ���ֽ�
			nonPicDataLength++;			//+1
			while (temp)				//ѭ����tempΪ0
			{
				fread(&temp, 1, 1, fp);	//�������0������һ�ֽڵ�����
				nonPicDataLength++;		//����
			}
			//������Description�ı����Լ�ĩβ��\0

			//������������+����ļ���ʽ
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
			//-----������ͼƬ����-----
			picLength = frameLength - nonPicDataLength;		//����ͼƬ���ݳ���
			pPicData = new byte[picLength];					//��̬����ͼƬ�����ڴ�ռ�
			memset(pPicData, 0, picLength);					//���ͼƬ�����ڴ�
			fread(pPicData, picLength, 1, fp);				//�õ�ͼƬ����
			//------------------------
			fclose(fp);										//��������ɣ��ر��ļ���
		}
		else if (id3v2h.major == 2)
		{
			//ID3v2.2
			ID3V22FrameHeader id3v2fh;		//����һ��ID3v2.2��ǩ֡ͷ�ṹ��
			memset(&id3v2fh, 0, 6);
			fread(&id3v2fh, 6, 1, fp);				//������д��ID3V2.2FrameHeader�ṹ����
			int curDataLength = 6;					//��ŵ�ǰ�Ѿ���ȡ�����ݴ�С���ղ��Ѿ�����6�ֽ�
			while ((strncmp(id3v2fh.FrameId, "PIC", 3) != 0))//���֡ͷû��PIC��ʶ����ѭ��ִ��
			{
				if (curDataLength > tagTotalLength)
				{
					fclose(fp);
					fp = NULL;
					return false;					//δ����ͼƬ����
				}
				//����֡���ݳ���
				int frameLength = id3v2fh.size[0] * 0x10000 + id3v2fh.size[1] * 0x100 + id3v2fh.size[2];
				fseek(fp, frameLength, SEEK_CUR);	//��ǰ��Ծ����һ��֡ͷ
				memset(&id3v2fh, 0, 6);			//���֡ͷ�ṹ������
				fread(&id3v2fh, 6, 1, fp);			//���¶�ȡ����
				curDataLength += frameLength + 6;	//��¼��ǰ���ڵ�ID3��ǩλ�ã��Ա��˳�ѭ��
			}

			int frameLength = id3v2fh.size[0] * 0x10000 + id3v2fh.size[1] * 0x100 + id3v2fh.size[2]; //���������ͼƬ֡������֡��

			/*
			���ݸ�ʽ��

			Attached picture   "PIC"
			Frame size         $xx xx xx
			Text encoding      $xx
			Image format       $xx xx xx
			Picture type       $xx
			Description        <textstring> $00 (00)
			Picture data       <binary data>
			*/

			int nonPicDataLength = 0;	//��ͼƬ���ݵĳ���
			fseek(fp, 1, SEEK_CUR);		//����֮Ծ Text encoding
			nonPicDataLength++;

			char imageType[4] = {};
			memset(&imageType, 0, 4);
			fread(&imageType, 3, 1, fp);//ͼ���ʽ
			nonPicDataLength += 3;

			fseek(fp, 1, SEEK_CUR);		//����֮Ծ Picture type
			nonPicDataLength++;

			int temp = 0;				//��¼��ǰ�ֽ����ݵı���
			fread(&temp, 1, 1, fp);		//��ȡһ���ֽ�
			nonPicDataLength++;			//+1
			while (temp)				//ѭ����tempΪ0
			{
				fread(&temp, 1, 1, fp);	//�������0������һ�ֽڵ�����
				nonPicDataLength++;		//����
			}
			//������Description�ı����Լ�ĩβ��\0

			//������������
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
			//-----������ͼƬ����-----
			picLength = frameLength - nonPicDataLength;		//����ͼƬ���ݳ���
			pPicData = new byte[picLength];					//��̬����ͼƬ�����ڴ�ռ�
			memset(pPicData, 0, picLength);					//���ͼƬ�����ڴ�
			fread(pPicData, picLength, 1, fp);				//�õ�ͼƬ����
			//------------------------
			fclose(fp);										//��������ɣ��ر��ļ���
		}
		else
		{
			//���಻֧�ֵİ汾
			fclose(fp);//�ر�
			fp = NULL;
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