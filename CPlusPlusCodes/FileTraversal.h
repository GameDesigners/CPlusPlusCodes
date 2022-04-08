#ifndef FILE_TRAVELSAL_H
#define FILE_TRAVELSAL_H
#include<stdio.h>
#include<io.h>
#include<string>
#include<vector>

namespace tools
{
	/*
	 *  此页面代码用于遍历某一个文件夹中所有的文件
	 */

	struct FileInfo
	{
		std::string abstractPath;
		__finddata64_t fileInfo;
	};

	/// <summary>
	/// 遍历文件夹中的所有文件（包含子文夹的文件）
	/// </summary>
	/// <param name="path">文件夹路径（绝对）</param>
	/// <param name="res">返回的文件路径集合</param>
	/// <returns>是否查找成功</returns>
	bool TraverseDirectory(std::string path, std::vector<FileInfo>& res)
	{
		__int64 handle;
		struct __finddata64_t fileinfo;
		std::string strFind = path + "\\*";

		if ((handle = _findfirst64(strFind.c_str(), &fileinfo)) == -1L)
		{
			printf("没有找到匹配的项目\n");
			return false;
		}

		do
		{
			//判断是否有子目录
			if (fileinfo.attrib & _A_SUBDIR)
			{
				if ((strcmp(fileinfo.name, ".") != 0) && (strcmp(fileinfo.name, "..") != 0))
				{
					std::string newPath = path + "\\" + fileinfo.name;
					TraverseDirectory(newPath, res);//递归
				}
			}
			else
			{
				std::string newPath = path + "\\" + fileinfo.name;
				//自定义操作
				FileInfo file = { newPath,fileinfo };
				res.push_back(file);
			}
		} while (_findnext64(handle, &fileinfo) == 0);
		_findclose(handle);
		return true;
	}

	const std::string lfs_suffix = " filter=lfs diff=lfs merge=lfs -text\n";

	void fileTraversalDemo()
	{
		//std::string inPath = "E:\\ZhuSenlin Project\\[0]Game Dev Projects\\UnityGameFrameworkDemo";
		std::string inPath = "E:\\Terrain-URP\\Assets";
		std::vector<FileInfo> files;
		int limit = 50 * 1024 * 1024;

		if (TraverseDirectory(inPath, files))
		{
			printf("检索成功，共检索了%d个文件\n", files.size());
		}
		std::string ouputs = "";
		for (int index = 0; index < files.size(); index++)
		{
			if (limit < files[index].fileInfo.size)
			{
				printf("%10lld   %s  \n", files[index].fileInfo.size, files[index].abstractPath.c_str());
				ouputs += files[index].fileInfo.name + lfs_suffix;
			}
		}

		printf("\n\nLFS　Format:\n");
		printf(ouputs.c_str());
	}
}
#endif // !FILE_TRAVELSAL_H
