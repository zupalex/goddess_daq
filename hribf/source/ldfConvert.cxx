#include "hribfBuffer.h"
#include <string>
#include <iostream>
#include <fstream>

int usage(std::string executable) {
	std::cout << "Usage: " << executable << " input.ldf output.geb\n";
	return 1; 
}

int main(int argc, char *argv[]) {
	if (argc != 3) {return usage(argv[0]);}
	std::string inputName = argv[1];
	std::string outputName = argv[2];

	hribfBuffer buffer(inputName.c_str());

	std::ofstream output(outputName,std::ofstream::binary);

	const int type = 19;
	
	int cnt=0;
	std::map<short, short> *values = buffer.GetMap();
	while (buffer.ReadNextBuffer() > 0) {
		//Print a buffer counter so the user sees that it is working.
		printf("Buffer: %d File: %5.2f%%\r",buffer.GetBufferNumber(),buffer.GetFilePositionPercentage());
		if (buffer.GetBufferNumber() % 100 == 0) {
			fflush(stdout);
		}

		if (buffer.GetBufferType() == hribfBuffer::BUFFER_TYPE_DATA) {
			while (buffer.GetEventsRemaining()) {
				buffer.ReadEvent();

				long long timestamp = 0;
				//timestamp = (values[500] << 32) | (values[501] << 16) | values[502];
				timestamp = ((long long) values->at(1) << 32) | (values->at(2) << 16) | values->at(3);

				int length = values->size() * 2 * sizeof(short);
				output.write((char*) &type,sizeof(int));
				output.write((char*) &length,sizeof(int));
				output.write((char*) &timestamp,sizeof(long long));
				for (auto itr=values->begin(); itr != values->end(); ++itr) {
					output.write((char*)&(itr->first), sizeof(short));
					output.write((char*)&(itr->second), sizeof(short));
				}
					
				cnt++;
			}
		}

	}
	std::cout << "Converted " << cnt << " events.           \n";

	return 0;
}
