

#include "json_xml_interconverter.h"

int main() {
	std::string jsonFileName("./resources/json/test.json");
	std::string xmlFileName("./resources/xml/xml_generated.xml");

	converter conv;

	std::string jsonStr;
	if (conv.readFile(jsonFileName, jsonStr)) {
		if (conv.json2Xml(jsonStr)) {
			std::cout << conv.getXmlStringFormat() << std::endl;
			std::cout << conv.getJsonStringFormat() << std::endl;
			conv.writeFile("./resources/xml/xml_generated.xml", 0);
		}
	}


	std::string xmlstr;
	if (conv.readFile(xmlFileName, xmlstr)) {
		if (conv.xml2Json(xmlstr)) {
			std::cout << conv.getXmlStringFormat() << std::endl;
			std::cout << conv.getJsonStringFormat() << std::endl;
			conv.writeFile("./resources/json/json_generated.json", 1);
		}
	}




	system("pause");
	return 0;
}
