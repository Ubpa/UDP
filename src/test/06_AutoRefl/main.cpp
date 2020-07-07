#include <UDP/tools/AutoRefl.h>

#include <fstream>

using namespace Ubpa;
using namespace std;

string readFileIntoString(const char* filename)
{
	ifstream ifile(filename);
	//将文件读入到ostringstream对象buf中
	ostringstream buf;
	char ch;
	while (buf && ifile.get(ch))
		buf.put(ch);
	//返回与流对象buf关联的字符串
	return buf.str();
}

int main() {
	auto code = readFileIntoString("../data/attr.h");

	AutoRefl autorefl;
	auto result = autorefl.Parse(code);

	ofstream out("../data/attr_out.h");
	out << result;
	out.close();

	return 0;
}
