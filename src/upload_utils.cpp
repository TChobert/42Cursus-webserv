#include "upload_utils.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

//fabriquer un nom unique simple
static std::string	generateUploadFileName(const std::string& ext)
{
	static int counter = 0;
	std::ostringstream ss;
	ss << "upload_" << counter++;
	if (!ext.empty())
		ss << ext;
	return ss.str();
}

//nettoie le nom du fichier, sinon genere un nouveau
//suppression caracteres non surs
static std::string cleanFilename(const std::string& raw, const std::string& ext)
{
	std::string cleanFilename;
	for (std::string::size_type i = 0; i < raw.size(); ++i)
	{
		if (std::isalnum(static_cast<unsigned char>(raw[i])) || raw[i] == '.' || raw[i] == '_' || raw[i] == '-')
		{
			cleanFilename += raw[i];
		}
	}
	return cleanFilename.empty() ? generateUploadFileName(ext) : cleanFilename;
}

void saveUploadedFile(const MultipartPart& part, Conversation& conv)
{
	std::string ext = getFileExtension(part.filename);
	std::string filename = cleanFilename(part.filename, ext);

	std::string path = conv.location->uploadStore + "/" + filename;

	std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
	if (!file.is_open())
		throw std::runtime_error("Impossible d'ouvrir le fichier en écriture");

	file.write(part.data.c_str(), static_cast<std::streamsize>(part.data.size()));
	file.close();

	conv.uploadedFiles.push_back(path);
}

void storeFormField(const MultipartPart& part, Conversation& conv)
{
	if (!part.data.empty())
		conv.formFields[part.name] = part.data;
}

void saveFormSummary(const Conversation& conv)
{
	static int counter = 0;
	std::ostringstream ss;
	ss << conv.location->uploadStore << "/upload_summary_" << counter++ << ".txt";

	std::ofstream out(ss.str().c_str());
	if (!out.is_open())
		throw std::runtime_error("Impossible d'ouvrir le fichier résumé");

	out << "=== Champs texte ===\n";
	for (std::map<std::string, std::string>::const_iterator it = conv.formFields.begin();
			it != conv.formFields.end(); ++it)
		out << it->first << ": " << it->second << "\n";

	if (!conv.uploadedFiles.empty())
	{
		out << "\n=== Fichiers uploadés ===\n";
		for (std::vector<std::string>::const_iterator it = conv.uploadedFiles.begin();
				it != conv.uploadedFiles.end(); ++it)
			out << *it << "\n";
	}
	out.close();
}
