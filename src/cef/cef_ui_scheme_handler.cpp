#include <std_include.hpp>

#include "cef/cef_ui_scheme_handler.hpp"

#include <utils/io.hpp>
#include <utils/string.hpp>

#define CEF_COMMAND "command"
#define CEF_DATA "data"

namespace cef
{
	namespace
	{
		std::string get_mime_type(const std::wstring& file)
		{
			auto extension = std::filesystem::path(file).extension().string();
			if (!extension.empty() && extension.front() == '.')
			{
				extension.erase(extension.begin());
			}

			const auto mime_type = CefGetMimeType(extension);
			if (!mime_type.empty())
			{
				return mime_type.ToString();
			}

			return "application/octet-stream";
		}
	}

	cef_ui_scheme_handler_factory::cef_ui_scheme_handler_factory(std::filesystem::path folder,
	                                                             const command_handlers& command_handlers)
		: folder_(std::move(folder))
		  , command_handlers_(command_handlers)
	{
	}

	CefRefPtr<CefResourceHandler> cef_ui_scheme_handler_factory::Create(CefRefPtr<CefBrowser> /*browser*/,
	                                                                    CefRefPtr<CefFrame> /*frame*/,
	                                                                    const CefString& /*scheme_name*/,
	                                                                    CefRefPtr<CefRequest> request)
	{
		CEF_REQUIRE_IO_THREAD();

		CefURLParts url_parts{};
		CefParseURL(request->GetURL(), url_parts);

		auto path = CefString(&url_parts.path).ToString();
		auto* const result = this->handle_command(request, path);
		if (result)
		{
			return result;
		}

		// Prevents files starting with a / from interacting badly with std::filesystem::path / concatenation
		while (path[0] == '/')
		{
			path.erase(path.begin());
		}

		const auto file = this->folder_ / path;
		std::string content;
		if (utils::io::read_file(file.string(), &content))
		{
			const auto& mime_type = get_mime_type(file);
			const auto stream = CefStreamReader::CreateForData(content.data(), content.size());
			return new CefStreamResourceHandler(mime_type, stream);
		}

		throw std::runtime_error("Could not read file at " + file.string());
	}

	CefResourceHandler* cef_ui_scheme_handler_factory::handle_command(const CefRefPtr<CefRequest>& request,
	                                                                  const std::string& path) const
	{
		if (path != "/command")
		{
			return nullptr;
		}

		CefPostData::ElementVector vector{};
		request->GetPostData()->GetElements(vector);

		assert(vector.size() == 1);
		const auto& element = vector.front();

		std::string json{};
		json.resize(element->GetBytesCount());
		element->GetBytes(json.size(), json.data());

		rapidjson::Document doc{};
		doc.Parse(json.data(), json.size());

		const auto& command = doc[CEF_COMMAND];
		const auto& data = doc[CEF_DATA];

		rapidjson::Document response{};
		response.SetObject();

		if (command.IsString())
		{
			auto command_name = std::string{command.GetString(), command.GetStringLength()};
			const auto handler = this->command_handlers_.find(command_name);
			if (handler != this->command_handlers_.end())
			{
				handler->second(data, response);
			}
		}

		rapidjson::StringBuffer buffer{};
		rapidjson::Writer<rapidjson::StringBuffer, rapidjson::Document::EncodingType, rapidjson::ASCII<>>
			writer(buffer);
		response.Accept(writer);

		json.assign(buffer.GetString(), buffer.GetLength());

		const auto stream = CefStreamReader::CreateForData(json.data(), json.size());
		return new CefStreamResourceHandler("application/json", stream);
	}
}
