

#include "json_xml.h"

int main() {
	std::string josnFileName("./resources/json/test.json");
	std::string xmlFileName("./resources/xml/test.xml");
	converter conv;
	//conv.json2Xml(josnFileName, josnFileName);
	conv.xml2Json(xmlFileName, xmlFileName);

	//std::string str;
	//_readFile(filename, str);
	//cJSON *root = cJSON_Parse(str.data);
	//cJSON_Print(root);

	system("pause");
	return 0;
}
