#pragma once

#include <Python.h>
#include <string>

using std::string;

/*****************************************************************************
* @class name : EmbedPython
* @author : chen
* @date : 2018/1/5 21:02
* @describe: 主要用于在c++程序中调用python-tensorflow程序的部分代码
			(需要预先定义系统全局变量：python环境路径例如：CPythonEnv __PYTHON_ENV_PATH__ = L"C:/Users/chen/Anaconda3/envs/py35";)
* @last change : 
*****************************************************************************/


using CPythonEnv = wchar_t *const;
extern CPythonEnv __PYTHON_ENV_PATH__;

class EmbedPython
{
private:
	EmbedPython()
	{
		Py_SetPythonHome(__PYTHON_ENV_PATH__);
		Py_Initialize();
	}
public:
	static EmbedPython* Get()
	{
		if (instance == nullptr)
			instance = new EmbedPython;
		return instance;
	}
	bool load_file_and_function(const string &filepath,const string &filename,const string &funcname)
	{
		string chdir_cmd = string("sys.path.append(\"");
		chdir_cmd = chdir_cmd  + filepath +  "\")";
		const char *cstr_cmd = chdir_cmd.c_str();

		PyRun_SimpleString("import sys");
		PyRun_SimpleString(cstr_cmd);

		File_module(PyImport_ImportModule(filename.c_str()));
		if (!File_module())
		{
			PyErr_Print();
			return false;
		}
		Func_module(PyObject_GetAttrString(File_module(), funcname.c_str()));

		if (!Func_module() || !PyCallable_Check(Func_module()))
		{
			PyErr_Print();
			return false;
		}
		return true;
	}
	bool exec_function(const string &arg0,long &result)
	{
		PyObject* args = PyTuple_New(1);
		PyObject* arg1 = Py_BuildValue("s", arg0.c_str());
		PyTuple_SetItem(args, 0, arg1);
		PyObject* pRet = PyObject_CallObject(Func_module(), args);
		if (pRet)
		{
			result = PyLong_AsLong(pRet);
		}
		return true;
	}
	~EmbedPython()
	{
		Py_Finalize();
	}
	PyObject * File_module() const { return file_module; }
	void File_module(PyObject * val) { file_module = val; }
	PyObject * Func_module() const { return func_module; }
	void Func_module(PyObject * val) { func_module = val; }
private:
	PyObject *file_module;
	PyObject *func_module;
	static EmbedPython *instance;
};


EmbedPython* EmbedPython::instance = nullptr;


