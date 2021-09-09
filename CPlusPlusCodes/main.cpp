#include<stdio.h>
#include<io.h>
#include<string>
#include<vector>
/*
 *  ��ҳ��������ڱ���ĳһ���ļ��������е��ļ� 
 */

struct FileInfo
{
	std::string abstractPath;
	__finddata64_t fileInfo;
};

/// <summary>
/// �����ļ����е������ļ����������ļе��ļ���
/// </summary>
/// <param name="path">�ļ���·�������ԣ�</param>
/// <param name="res">���ص��ļ�·������</param>
/// <returns>�Ƿ���ҳɹ�</returns>
bool TraverseDirectory(std::string path,std::vector<FileInfo>& res)
{
	__int64 handle;
	struct __finddata64_t fileinfo;
	std::string strFind = path + "\\*";

	if((handle = _findfirst64(strFind.c_str(),&fileinfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ\n");
		return false;
	}

	do
	{
		//�ж��Ƿ�����Ŀ¼
		if (fileinfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(fileinfo.name, ".") != 0) && (strcmp(fileinfo.name, "..") != 0))
			{
				std::string newPath = path + "\\" + fileinfo.name;
				TraverseDirectory(newPath,res);//�ݹ�
			}
		}
		else
		{
			std::string newPath = path + "\\" + fileinfo.name;
			//�Զ������
			FileInfo file = { newPath,fileinfo };
			res.push_back(file);
		}
	}
	while (_findnext64(handle, &fileinfo) == 0);
	_findclose(handle);
	return true;
}

int main()
{
	std::string inPath = "E:\\ZhuSenlin Project\\[0]Game Dev Projects\\UnityGameFrameworkDemo";
	std::vector<FileInfo> files;
	int limit = 100 * 1024 * 1024;

	if (TraverseDirectory(inPath, files))
	{
		printf("�����ɹ�����������%d���ļ�\n", files.size());
	}

	for (int index = 0; index < files.size(); index++)
	{
		if (limit < files[index].fileInfo.size)
			printf("%10lld   %s  \n", files[index].fileInfo.size, files[index].abstractPath.c_str());
	}
}

