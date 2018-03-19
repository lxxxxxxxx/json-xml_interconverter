
#ifndef _JSON_XML_INTERCONVERTER_H_
#define _JSON_XML_INTERCONVERTER_H_

#include <fstream>
#include <string>
#include <iostream>
#include <cctype>

#include "cjson\cJSON.h"
#include "tinyxml2\tinyxml2.h"

const size_t buf_size = 16 * 1024 * 1024;

class converter {
public:
	converter();
	~converter();

	/*   read file to 'str' all at once   */
	bool readFile(const std::string &filename, std::string &str);

	/*   write json/xml to file , the second atgument is '1' if write json,
	    '0' if write xml   */
	bool writeFile(const std::string &path, bool writeJson);

	/*   convert json string to xml   */
	bool json2Xml(const std::string &jsonStr);

	/*   convert xml string to json   */
	bool xml2Json(const std::string &xmlStr);

	/*   return format json string   */
	std::string getJsonStringFormat();

	/*   return unformat json string   */
	std::string getJsonStringUnformat();

	/*   return format xml string   */
	std::string getXmlStringFormat();

	/*   return unformat json string   */
	std::string getXmlStringUnformat();

private:
	/*   parse json string to json tree   */
	bool parseJson(const std::string &jsonStr);

	/*   parse xml string to xml doc   */
	bool parseXml(const std::string &xmlStr);

	/*   traverse json tree,construct a xml doc   */
	void jsonNode2xmlNode(cJSON *jsonNode, tinyxml2::XMLNode *xmlNode, bool isArray, const std::string &arrNaem);

	/*   traverse xml doc,construct json tree   */
	void xmlNode2jsonNode(tinyxml2::XMLElement *xmlEle, cJSON *jsonNode);

private:
	cJSON *jsonRoot;    //json
	tinyxml2::XMLDocument doc;   //xml
};


//#################################################################

converter::converter():jsonRoot(nullptr){
}

converter::~converter(){
	cJSON_Delete(jsonRoot);
	doc.Clear();
}

bool converter::readFile(const std::string &filename, std::string &str) {
	std::ifstream file;
	char *buf = nullptr;
	std::size_t filesize = 0;

	file.open(filename, std::ios::in | std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		std::cout << "open \"" << filename << "\" failed" << std::endl;
		return false;
	}

	filesize = file.tellg();
	if (filesize > buf_size) {
		std::cout << "warning: file too large,expect a file smaller than 16M." << std::endl;
		filesize = buf_size;
	}
	file.seekg(std::ios::beg);

	try {
		buf = new char[buf_size];
	}
	catch (std::bad_alloc &e) {
		std::cerr << "bad_alloc:" << e.what() << std::endl;
		return false;
	}
	memset(buf, '\0', buf_size);

	file.read(buf, filesize);
	file.close();
	str.assign(buf);
	delete[]buf;

	return true;;
}

bool converter::writeFile(const std::string &path, bool json) {
	std::ofstream file;
	file.open(path, std::ios::out | std::ios::beg|std::ios::binary);
	if (!file.is_open()) {
		std::cout << "file \"" << path << " \"open failed." << std::endl;
		return false;
	}
	const char *tmp = nullptr;
	if (json) {
		const char *tmp = cJSON_Print(jsonRoot);
		//std::cout << "befor write: " << tmp << std::endl;
		file.write(tmp, strlen(tmp));
		file.close();
	}
	else {
		tinyxml2::XMLPrinter printer;
		doc.Print(&printer);
		//std::cout << "befor write: " << printer.CStr() << std::endl;
		file.write(printer.CStr(), printer.CStrSize());
		file.close();
	}
	return true;

}




bool converter::parseJson(const std::string &jsonStr) {
	jsonRoot = cJSON_Parse(jsonStr.data());
	if (jsonRoot == nullptr) {
		std::cout << "json parse failed befor " << cJSON_GetErrorPtr() << std::endl;
		return false;
	}
	return true;
}

void converter::jsonNode2xmlNode(cJSON *jsonNode,tinyxml2::XMLNode *xmlNode,bool arrChild,const std::string &arrName) {
	if (!jsonNode) {return;}
	tinyxml2::XMLElement *newElement = nullptr;

	switch (jsonNode->type) {
	case cJSON_False:
		if (arrChild) {
			newElement = doc.NewElement(arrName.data());
		}
		else {
			newElement = doc.NewElement(jsonNode->string);
		}
		newElement->InsertEndChild(doc.NewText("false"));
		xmlNode->InsertEndChild(newElement);

		if (jsonNode->next) {
			if (arrChild) {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 1, arrName);
			}
			else {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 0, "");
			}
		}
		break;

	case cJSON_True:
		if (arrChild) {
			newElement = doc.NewElement(arrName.data());
		}
		else {
			newElement = doc.NewElement(jsonNode->string);
		}

		newElement->InsertEndChild(doc.NewText("true"));
		xmlNode->InsertEndChild(newElement);

		if (jsonNode->next) {
			if (arrChild) {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 1, arrName);
			}
			else {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 0, "");
			}
		}

		break;


	case cJSON_Number:
		if (arrChild) {
			newElement = doc.NewElement(arrName.data());
		}
		else {
			newElement = doc.NewElement(jsonNode->string);
		}

		if (jsonNode->valuedouble == jsonNode->valueint) {
			newElement->InsertEndChild(doc.NewText(std::to_string(jsonNode->valueint).data()));
		}
		else {
			newElement->InsertEndChild(doc.NewText(std::to_string(jsonNode->valuedouble).data()));
		}
		xmlNode->InsertEndChild(newElement);
		if (jsonNode->next) {
			if (arrChild) {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 1, arrName);

			}
			else {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 0, "");

			}
		}
		break;

	case cJSON_String:
		if (arrChild) {
			newElement = doc.NewElement(arrName.data());
		}
		else {
			newElement = doc.NewElement(jsonNode->string);
		}

		newElement->InsertEndChild(doc.NewText(jsonNode->valuestring));
		xmlNode->InsertEndChild(newElement);

		if (jsonNode->next) {
			if (arrChild)
				jsonNode2xmlNode(jsonNode->next, xmlNode, 1, arrName);
			else
				jsonNode2xmlNode(jsonNode->next, xmlNode, 0, "");
		}
		break;

	case cJSON_Array:
		if (jsonNode->child) {
			if (arrChild) {
				jsonNode2xmlNode(jsonNode->child, xmlNode, 1, arrName);
			}
			else {
				jsonNode2xmlNode(jsonNode->child, xmlNode, 1, std::string(jsonNode->string));
			}
		}
		if (jsonNode->next) {
			if (arrChild) {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 1, arrName);
			}
			else {
				jsonNode2xmlNode(jsonNode->next, xmlNode, 1, std::string(jsonNode->string));
			}
		}
		break;

	case cJSON_Object:
		
		if (jsonNode->string) {
			//if array can own a not null json object chaild ?
			if (arrChild) {
				newElement = doc.NewElement(arrName.data());
			}
			else {
				newElement = doc.NewElement(jsonNode->string);
			}
			//the begin of json object is a object
			if (xmlNode == nullptr) {
				doc.InsertEndChild(newElement);
				doc.InsertFirstChild(doc.NewDeclaration());
			}
			else {
				xmlNode->InsertEndChild(newElement);
			}
		}

		if (jsonNode->child) {
			jsonNode2xmlNode(jsonNode->child, newElement,0,"");
		}
		if (jsonNode->next) {
			jsonNode2xmlNode(jsonNode->next, xmlNode,0,"");
		}
		break;
	case cJSON_Raw:
	case cJSON_Invalid:
	case cJSON_NULL:
	default:
		break;
	}
}

bool converter::json2Xml(const std::string &jsonStr) {
	doc.Clear();
	if (!parseJson(jsonStr)) {
		return false;
	}
	jsonNode2xmlNode(jsonRoot, doc.RootElement(), 0, "");
	return true;
}

std::string converter::getJsonStringFormat() {
	if (jsonRoot == nullptr) {
		return "json empty..";
	}
	char *tmp = cJSON_Print(jsonRoot);
	std::string printed(tmp);
	if (tmp != nullptr) {
		free(tmp);
	}
	return printed;
}

std::string converter::getJsonStringUnformat() {
	if (jsonRoot == nullptr) {
		return "json empty..";
	}
	char *tmp = cJSON_PrintUnformatted(jsonRoot);
	std::string printed(tmp);
	if (tmp != nullptr) {
		free(tmp);
	}
	return printed;
}




bool converter::parseXml(const std::string &xmlStr) {
	doc.Parse(xmlStr.data());
	if (doc.Error()) {
		std::cout << "xml parse error: " << doc.ErrorStr() << std::endl;
		return false;
	}
	return true;
}

void converter::xmlNode2jsonNode(tinyxml2::XMLElement *xmlEle, cJSON *jsonNode) {
	const char * _key = xmlEle->Name();
	const char * _value = xmlEle->GetText();
	cJSON *newNode = nullptr;
	//当key不为空时才处理
	if (_key) {
		//如果value不为空
		if (_value != nullptr) {
			//判断当前节点是不是array的元素
			std::size_t arrElementCount = 1;
			auto *tmp = xmlEle->NextSiblingElement();
			while (tmp && (0==strcmp(_key ,tmp->Name()))) {
				arrElementCount++;
				tmp = tmp->NextSiblingElement();
			}
			//多个xml元素名一样，说明是一个array
			if (arrElementCount > 1) {
				cJSON *newArr = cJSON_CreateArray();

				tinyxml2::XMLElement *arrEle = xmlEle;
				const char *str = nullptr;
				for (int i = 0; i < arrElementCount; i++) {
					str = arrEle->GetText();
					cJSON_AddItemToArray(newArr, cJSON_CreateString(str));
					arrEle = arrEle->NextSiblingElement();
				}
				cJSON_AddItemToObject(jsonNode, _key, newArr);
				//char *printed = cJSON_Print(jsonNode);
				//std::cout << printed << std::endl;
			}
			//不是array
			else {
				//如果有元素属性，则处理元素属性，新建一个node，并将每个元素属性作为一个节点插入到该节点下
				const tinyxml2::XMLAttribute *attr = xmlEle->FirstAttribute();
				if (attr) {
					std::string attrName, attrValue;
					cJSON *newNode = cJSON_CreateObject();
					while (attr) {
						attrName = "-";
						attrName.append(attr->Name());
						attrValue = attr->Value();
						cJSON_AddStringToObject(newNode, attrName.data(), attrValue.data());
						attr = attr->Next();
					}
					cJSON_AddStringToObject(newNode, "#text", _value);
					cJSON_AddItemToObject(jsonNode,_key, newNode);
				}
				//没有元素属性，将key和value作为一个string对象添加到jsonNode下
				else {
					cJSON_AddStringToObject(jsonNode, _key, _value);
				}
				//char *printed = cJSON_Print(jsonRoot);
				//std::cout << printed << std::endl;
			}
			//递归遍历array之后的下一个节点
			if (tmp) {
				xmlNode2jsonNode(tmp, jsonNode);
			}
		}

		//若value为空，则说明该元素的值是其他对象
		else {
			cJSON *newNode = cJSON_CreateObject();
			cJSON_AddItemToObject(jsonNode, _key, newNode);
			//char *tmp = cJSON_Print(jsonNode);
			//std::cout << tmp << std::endl;
			if (xmlEle->FirstChildElement()) {
				xmlNode2jsonNode(xmlEle->FirstChildElement(), newNode);
			}
			if (xmlEle->NextSiblingElement()) {
				xmlNode2jsonNode(xmlEle->NextSiblingElement(), jsonNode);
			}
		}
	}
}

bool converter::xml2Json(const std::string &xmlStr) {	
	if (jsonRoot) {
		cJSON_Delete(jsonRoot);
	}
	jsonRoot = cJSON_CreateObject();

	if (!parseXml(xmlStr)) {
		return false;
	}
	xmlNode2jsonNode(doc.RootElement(), jsonRoot);

	return true;
}

std::string converter::getXmlStringFormat() {
	if (nullptr == doc.RootElement()) {
		return "xml empty..";
	}
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);
	return printer.CStr();
}

std::string converter::getXmlStringUnformat() {
	if (nullptr == doc.RootElement()) {
		return "xml empty..";
	}
	tinyxml2::XMLPrinter printer(0, 1, 0);
	doc.Print(&printer);
	return printer.CStr();
}

#endif