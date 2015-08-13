#include "hribfBuffer.h"
#include <string>
#include <iostream>
#include <fstream>

int usage(std::string executable) {
	std::cout << "Usage: " << executable << " input.ldf output.geb\n";
	return 1; 
}

int main(int argc, char *argv[]) {
	const bool verbose = false;

	if (argc != 3) {return usage(argv[0]);}
	std::string inputName = argv[1];
	std::string outputName = argv[2];

	hribfBuffer buffer(inputName.c_str());

	std::ofstream output(outputName,std::ofstream::binary);

	const int type = 19;

	//The parameter numbers of the MYRIAD timestamp, low to high bits.
	const unsigned short myriadParam[3]  = {1002,1001,1000};
	
	unsigned int eventCnt=0;
	unsigned int myriadFailCnt = 0;
	std::map<unsigned short, unsigned short> *values = buffer.GetMap();
	while (buffer.ReadNextBuffer() > 0) {

		//Print a buffer counter so the user sees that it is working.
		printf("Buffer: %d File: %5.2f%%\r",buffer.GetBufferNumber(),buffer.GetFilePositionPercentage());
		if (buffer.GetBufferNumber() % 100 == 0) {
			fflush(stdout);
		}

		if (buffer.GetBufferType() == hribfBuffer::BUFFER_TYPE_DATA) {
			while (buffer.GetEventsRemaining()) {
				eventCnt++;

				buffer.ReadEvent();

				unsigned long long int timestamp = 0;
				bool myriadFail = false;
				for (int i=0;i<3;i++) {
					if (values->find(myriadParam[i]) == values->end()) {
						std::cerr << "ERROR: Timestamp not found in event! Parameters checked";
						for (int param=0;param < 3;param++) 	
							std::cerr << ", " << myriadParam[param];
						std::cerr << ".\n";
						std::cerr << "\tFor your reading pleasure an event dump:\n";
						for (auto itr=values->begin(); itr != values->end(); ++itr) {
							std::cerr << "\tValue[" << itr->first << "]=" << itr->second << "\n";
						}
						myriadFail = true;
						break;
					}

					timestamp |= (unsigned long long) values->at(myriadParam[i]) << (i * 16);
				}

				if (myriadFail) continue;

				if (values->at(myriadParam[2]) == 0xAAAA) {
					myriadFailCnt++;

					if (verbose) {
						std::cout << "\n";
						for (auto itr=values->begin(); itr != values->end(); ++itr) {
							std::cout << "\tValue[" << itr->first << "]=" << std::hex << itr->second << "\n";
						}
						std::cout << std::hex << timestamp << "\n";
					}

					continue;
				}

				int length = values->size() * 2 * sizeof(short);
				output.write((char*) &type,sizeof(int));
				output.write((char*) &length,sizeof(int));
				output.write((char*) &timestamp,sizeof(long long));
				for (auto itr=values->begin(); itr != values->end(); ++itr) {
					output.write((char*)&(itr->first), sizeof(short));
					output.write((char*)&(itr->second), sizeof(short));
				}

			}
		}

	}
	std::cout << "Converted " << eventCnt << " events.           \n";
	std::cout << "Malformed Myriad Events: " << myriadFailCnt << " " << (float)myriadFailCnt*100/eventCnt << "%\n";

	return 0;
}
