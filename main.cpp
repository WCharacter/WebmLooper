#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

void replace_time_value(string& bytes, const char* replace_bytes, uint32_t bytes_length)
{
	for (int i = 0; i < bytes.length(); ++i)
	{
		if (static_cast<uint8_t>(bytes[i]) == 0x44 && i+1 < bytes.length())
		{
			if (static_cast<uint8_t>(bytes[i + 1]) == 0x89)
			{
				
				if (i + 2 + bytes_length < bytes.length())
				{
					copy(replace_bytes, replace_bytes + bytes_length, bytes.begin() + i + 2);
					return;
				}
			}
		}
	}

	throw runtime_error("Time bytes not found");
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: webmlooper.exe <filename> [<filename 2> ...]";
		return 0;
	}

	char bytes[] = { 0x88, 0x3f, 0xf0, 0x00, 0x00, 0x00 }; // set webm duration to 0

	for (int i = 1; i < argc; ++i)
	{
		string filename = argv[i];

		cout << "Looping file " << argv[i] << '\n';
		ifstream input{ argv[i], ios_base::binary};

		try
		{
			ostringstream str_data;
			str_data << input.rdbuf();
			string all_bytes = str_data.str();
			replace_time_value(all_bytes, bytes, sizeof(bytes));

			uint32_t filename_length = filename.length() - (filename.length() - 5); //5 letters for .webm
			string output_filename{ filename.substr(0,filename_length) + "_" + to_string(i) + ".webm" };
			ofstream output{ output_filename, ios_base::binary };

			output.write(all_bytes.c_str(), all_bytes.length());
			output.close();

			cout << "File " << filename << " succesfully edited and saved as " << output_filename << '\n';
		}
		catch (const runtime_error& re)
		{
			cerr << re.what() << '\n';
		}

		input.close();
	}

	return 0;
}