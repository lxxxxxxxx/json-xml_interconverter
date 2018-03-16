

#include "json_xml.h"

int main() {
	std::string jsonFileName("./resources/json/test.json");
	std::string xmlFileName("./resources/xml/test.xml");
	converter conv;
	//std::cout << conv.getXmlStringFormat() << std::endl;
	//std::cout << conv.getJsonStringFormat() << std::endl;
	//std::string jsonstr;
	//conv.readFile(jsonFileName, jsonstr);
	//if (conv.json2Xml(jsonstr)) {
	//	conv.writeFile("./resources/json_generated.xml", 1);
	//	conv.writeFile("./resources/xml_generated.xml", 0);
	//	std::cout << conv.getXmlStringFormat() << std::endl;
	//	std::cout << conv.getJsonStringFormat() << std::endl;
	//}

	std::string xmlstr;
	conv.readFile(xmlFileName, xmlstr);
	if (conv.xml2Json(xmlstr)) {
		//conv.writeFile("./resources/json_generated.xml", 1);
		//conv.writeFile("./resources/xml_generated.xml", 0);

		//std::cout << conv.getXmlStringFormat() << std::endl;
		//std::cout << conv.getJsonStringFormat() << std::endl;
	}



	system("pause");
	return 0;
}
