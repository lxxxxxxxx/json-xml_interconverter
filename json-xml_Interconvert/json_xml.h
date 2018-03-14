


#ifndef _JSON_LX_H_
#define _JSON_LX_H_

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



	bool json2Xml(std::string infile, std::string outfile);
	bool xml2Json(std::string infile, std::string outfile);

private:

	/*read the whole file to memory one time*/
	bool _readFile(std::string filename, std::string &str);

	void parseJsonNode(cJSON *node, tinyxml2::XMLDocument *doc );

	void parseXmlNode(tinyxml2::XMLDocument *doc, cJSON *node);

	bool _getJsonFromFile(std::string filename);

	bool _getXmlFromFile(std::string filename);

	bool _writeJsonFile(std::string path);

	bool _writeXmlFile(std::string path);

private:
	cJSON *root;
	tinyxml2::XMLDocument doc;
};




//#################################################################


converter::converter():root(nullptr) {}

bool converter::_readFile(std::string filename, std::string &str) {
	std::fstream file;
	char *buf = nullptr;
	std::size_t filesize = 0;

	file.open(filename, std::ios::in | std::ios::ate|std::ios::binary);
	if (!file.is_open()) {
		std::cout << "open \""<<filename<<"\" failed" << std::endl;
		return false;
	}

	filesize = file.tellg();
	//std::cout << filesize << std::endl;
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
	str.assign(buf);
	delete[]buf;

	return true;;
}

void converter::parseJsonNode(cJSON *node,tinyxml2::XMLDocument *doc) {
	if (!node) {
		return;
	}

	std::string _key;
	std::string _value_string;
	double _value_double;

	switch (node->type) {
	case cJSON_Invalid:
		std::cout << "--------------------json invalid" << std::endl;
		break;

	case cJSON_False:
		std::cout << "-------------------json false" << std::endl;
		break;

	case cJSON_True:
		std::cout << "-------------------json true" << std::endl;
		break;

	case cJSON_NULL:
		std::cout << "------------------json null" << std::endl;
		break;

	case cJSON_Number:
		std::cout << "--------------------json number" << std::endl;
		//....construct xml node with number
		if (node->string) {
			_key.assign(node->string);
		}
		_value_double = node->valuedouble;
		std::cout << _key << " : " << _value_double << std::endl;
		break;

	case cJSON_String:
		std::cout << "--------------------josn string" << std::endl;
		//....construct xml node with string
		if (node->string) {
			_key.assign(node->string);
		}
		if (node->valuestring) {
			_value_string.assign(node->valuestring);
		}
		std::cout << _key << " : " << _value_string << std::endl;
		break;

	case cJSON_Array:
		std::cout << "----------------json array" << std::endl;
		if (node->string) {
			_key.assign(node->string);
		}
		break;

	case cJSON_Object:
		std::cout << "---------------json object" << std::endl;
		if (node->string) {
			_key.assign(node->string);
			std::cout << _key << std::endl;
		}
		break;

	case cJSON_Raw:
		std::cout << "------------------json raw" << std::endl;
		break;
	default:
		std::cout << "fatal: no such type,cjson parse error.";
		break;
	}


	if (node->child) {
		parseJsonNode(node->child, doc);
	}
	if (node->next) {
		parseJsonNode(node->next, doc);
	}
}

void converter::parseXmlNode(tinyxml2::XMLDocument *doc, cJSON *node) {
	tinyxml2::XMLElement *root = doc->RootElement();

	//TODO:





}


bool converter::_getJsonFromFile(std::string filename) {
	std::string str;
	if (_readFile(filename, str)) {
		root = cJSON_Parse(str.data());
		//std::cout << str << std::endl;
		//char * tmp = cJSON_Print(root);
		//std::cout << "------------------------------------\n";
		//std::cout << tmp << std::endl;
		return true;
	}
	return false;
}


bool converter::_getXmlFromFile(std::string filename) {
	doc.LoadFile(filename.data());
	return false;
}


bool converter::json2Xml(std::string infile,std::string outfile) {
	_getJsonFromFile(infile);
	parseJsonNode(root, &doc);




	return true;
}


bool converter::xml2Json(std::string infile, std::string outfile) {
	_getXmlFromFile(infile);
	
	doc.Print();
	return true;
}


#endif