#include "GoddessToolbox.h"

string pathToGDAQ;

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

string GetLocalTimeAndDate(bool doPrint)
{
	time_t t = time(nullptr);
	struct tm* localt = localtime(&t);

	char* timeAndDate = new char[256];

	sprintf(timeAndDate, "%s", asctime(localt));

	string timeAndDateStr = FindAndReplaceInString(timeAndDate, "\n", "");

	if (doPrint) cout << timeAndDateStr << endl;

	return timeAndDateStr;
}

string GetCurrentYear(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t lastSpace = timeAndDate.find_last_of(" ");

	string currYear = timeAndDate.substr(lastSpace + 1);

	currYear = FindAndReplaceInString(currYear, "\n", "");

	if (doPrint) std::cout << currYear << endl;

	return currYear;
}

string GetCurrentDayName(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t firstSpace = timeAndDate.find_first_of(" ");

	string currDay = timeAndDate.substr(0, firstSpace);

	if (doPrint) std::cout << currDay << endl;

	return currDay;
}

string GetCurrentMonthName(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t firstSpace = timeAndDate.find_first_of(" ");
	size_t secSpace = timeAndDate.find_first_of(" ", firstSpace + 1);

	string currMonth = timeAndDate.substr(firstSpace + 1, secSpace - firstSpace - 1);

	if (doPrint) std::cout << currMonth << endl;

	return currMonth;
}

int GetCurrentDayNum(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t firstNum = timeAndDate.find_first_of("0123456789");
	size_t nextSpace = timeAndDate.find_first_of(" ", firstNum + 1);

	string currDayNum = timeAndDate.substr(firstNum, firstNum - nextSpace);

	if (doPrint) std::cout << currDayNum << endl;

	return std::stoi(currDayNum);
}

int GetCurrentMonthNum()
{
	string currMonthName = GetCurrentMonthName();

	if (currMonthName == "Jan") return 1;
	else if (currMonthName == "Feb") return 2;
	else if (currMonthName == "Mar") return 3;
	else if (currMonthName == "Apr") return 4;
	else if (currMonthName == "May") return 5;
	else if (currMonthName == "Jun") return 6;
	else if (currMonthName == "Jul") return 7;
	else if (currMonthName == "Aug") return 8;
	else if (currMonthName == "Sep") return 9;
	else if (currMonthName == "Oct") return 10;
	else if (currMonthName == "Nov") return 11;
	else if (currMonthName == "Dec") return 12;
	else return 0;
}

int GetCurrentHour(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t firstNum = timeAndDate.find_first_of("0123456789");
	size_t nextSpace = timeAndDate.find_first_of(" ", firstNum + 1);
	size_t firstSemicolon = timeAndDate.find_first_of(":", firstNum + 1);

	string currHour = timeAndDate.substr(nextSpace + 1, firstSemicolon - nextSpace - 1);

	if (doPrint) std::cout << currHour << endl;

	return std::stoi(currHour);
}

int GetCurrentMinute(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t firstSemicolon = timeAndDate.find_first_of(":");
	size_t secSemicolon = timeAndDate.find_first_of(":", firstSemicolon + 1);

	string currMinute = timeAndDate.substr(firstSemicolon + 1, secSemicolon - firstSemicolon - 1);

	if (doPrint) std::cout << currMinute << endl;

	return std::stoi(currMinute);
}

int GetCurrentSecond(bool doPrint)
{
	string timeAndDate = GetLocalTimeAndDate();

	size_t firstSemicolon = timeAndDate.find_first_of(":");
	size_t secSemicolon = timeAndDate.find_first_of(":", firstSemicolon + 1);
	size_t nextSpace = timeAndDate.find_first_of(" ", secSemicolon + 1);

	string currMinute = timeAndDate.substr(secSemicolon + 1, nextSpace - secSemicolon - 1);

	if (doPrint) std::cout << currMinute << endl;

	return std::stoi(currMinute);
}

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

bool CompareStrings(string ref_, string search_, bool exactMatch, bool caseSensitive)
{
	if (std::search(ref_.begin(), ref_.end(), search_.begin(), search_.end(), ignoreCharCasePred(caseSensitive)) != ref_.end())
	{
		if (exactMatch && ref_.length() != search_.length()) return false;
		else return true;
	}

	return false;
}

std::vector<int> DecodeNumberString(std::string itemsString, bool verbose)
{
	std::vector<int> sectorsNum;

	sectorsNum.clear();

	while (itemsString.length() > 0)
	{
		size_t foundComa = itemsString.find(",");
		size_t foundDash = itemsString.find("-");

		std::string sub1 = "";
		std::string sub2 = "";

		if (foundComa != std::string::npos && foundComa < foundDash)
		{
			sub1 = itemsString.substr(0, foundComa);
		}
		else if (foundDash != std::string::npos)
		{
			sub1 = itemsString.substr(0, foundDash);
			sub2 = itemsString.substr(foundDash + 1, foundComa - foundDash - 1);
		}
		else sub1 = itemsString;

		int fstPush;

		if (sub1.length() > 0)
		{
			while (sub1.find(" ") != std::string::npos)
			{
				sub1.replace(sub1.find(" "), 1, "");
			}

			bool isDigit = true;

			for (unsigned int i = 0; i < sub1.length(); i++)
			{
				if (!isdigit(sub1[i])) isDigit = false;
			}

			if (isDigit) fstPush = stoi(sub1);
			else
			{
				fstPush = (int) sub1[0] - (int) ('A');

				if (sub1.length() > 1 || fstPush > 25)
				{
					cerr << "Invalid string to decode\n";

					sectorsNum.clear();

					return sectorsNum;
				}
			}

			sectorsNum.push_back(fstPush);

			if (sub2.length() > 0)
			{
				while (sub2.find(" ") != std::string::npos)
				{
					sub2.replace(sub2.find(" "), 1, "");
				}

				int lowerBound = fstPush;
				int upperBound;

				isDigit = true;

				for (unsigned int i = 0; i < sub2.length(); i++)
				{
					if (!isdigit(sub2[i])) isDigit = false;
				}

				if (isDigit) upperBound = stoi(sub2);
				else
				{
					upperBound = (int) sub2[0] - (int) ('A');

					if (sub2.length() > 1 || upperBound > 25 || upperBound < lowerBound)
					{
						cerr << "Invalid string to decode\n";

						sectorsNum.clear();

						return sectorsNum;
					}
				}

				for (int i = lowerBound + 1; i < upperBound; i++)
				{
					sectorsNum.push_back(i);
				}
			}
		}

		if (foundComa != std::string::npos || foundDash != std::string::npos)
		{
			//             std::cout << "itemsString was " << itemsString;
			itemsString.replace(0, sub2.length() > 0 ? foundDash + 1 : foundComa + 1, "");
			//             std::cout << " ... is now " << itemsString << "\n";
		}
		else
		{
			itemsString.clear();
		}
	}

	if (verbose)
	{
		std::cout << "Decoded the sectors string:\n";

		for (unsigned int i = 0; i < sectorsNum.size(); i++)
		{
			std::cout << "-> " << sectorsNum[i] << "\n";
		}
	}

	return sectorsNum;
}

std::vector<std::string> DecodeTags(std::string tagsStr)
{
	std::vector<std::string> tags;

	std::size_t startPos = tagsStr.find_first_not_of(" ,");
	;

	std::size_t separator = tagsStr.find_first_of(" ,", startPos);

	std::string newTag;

	while (separator != std::string::npos && startPos != std::string::npos)
	{
		newTag = tagsStr.substr(startPos, separator - startPos);

		tags.push_back(newTag);

		startPos = tagsStr.find_first_not_of(" ,", separator);
		separator = tagsStr.find_first_of(" ,", startPos);
	}

	if (startPos != std::string::npos)
	{
		newTag = tagsStr.substr(startPos, tagsStr.length() - startPos);

		tags.push_back(newTag);
	}

	return tags;
}

vector<string> GetDirContent(string dirName, string mode, string endWith, string mustHaveAll, string cantHaveAny, string mustHaveOneOf, string startWith,
		bool caseSensitive, bool includePath)
{
	std::vector<std::string> fileList, mustAllTags, cantTags, mustOneOfTags, endWithVec;

	if (!mustHaveAll.empty())
	{
		mustAllTags = DecodeTags(mustHaveAll);
	}

	if (!cantHaveAny.empty())
	{
		cantTags = DecodeTags(cantHaveAny);
	}

	if (!mustHaveOneOf.empty())
	{
		mustOneOfTags = DecodeTags(mustHaveOneOf);

		if (mustOneOfTags.back() == "\\shouldfinishwith")
		{
			mustOneOfTags.erase(mustOneOfTags.end() - 1);

			for (unsigned int i = 0; i < mustOneOfTags.size(); i++)
				endWithVec.push_back(mustOneOfTags[i]);
		}
	}

	if (startWith == "*") startWith.clear();

	DIR* toScan = nullptr;
	dirent* dirEntry = nullptr;

	int scanMode = 0;

	if (mode.find("system") == 0)
	{
		if (dirName.empty()) dirName = "./";
		toScan = opendir(dirName.c_str());
		dirEntry = new dirent;

		scanMode = 1;
	}

	TIterator* rootListItr = nullptr;
	TObject* rootListEntry = nullptr;

	std::vector<string> acceptedClasses;
	acceptedClasses.clear();

	if (mode.find("root") == 0)
	{
		if (mode.length() > 4)
		{
			std::size_t classesSep = mode.find(" ");

			while (classesSep != std::string::npos)
			{
				std::size_t classStartPos = mode.find_first_not_of(" \b\r\n\v", classesSep);

				if (classStartPos == string::npos) break;

				std::size_t nextClassesSep = mode.find(" ", classStartPos);

				if (nextClassesSep != std::string::npos) acceptedClasses.push_back(mode.substr(classStartPos, nextClassesSep - classStartPos));
				else acceptedClasses.push_back(mode.substr(classStartPos));

				classesSep = nextClassesSep;
			}

			if (acceptedClasses.size() > 0) cout << "Class filters: ";
			for (unsigned int cFilt = 0; cFilt < acceptedClasses.size(); cFilt++)
			{
				cout << "\"" << acceptedClasses[cFilt] << "\" ";
			}
			cout << endl;
		}

		if (!dirName.empty()) gDirectory->cd(dirName.c_str());

		TList* gDirList = gDirectory->GetList();
		TList* gDirKeys = gDirectory->GetListOfKeys();

		TList* gDirAllObj = new TList();
		gDirAllObj->Clear();

		if (gDirList != nullptr && gDirList->GetSize() > 0) gDirAllObj->AddAll(gDirList);
		if (gDirKeys != nullptr && gDirKeys->GetSize() > 0) gDirAllObj->AddAll(gDirKeys);

		if (gDirAllObj != nullptr && gDirAllObj->GetSize() > 0)
		{
			rootListItr = gDirAllObj->MakeIterator();
			rootListEntry = rootListItr->Next();
		}

		scanMode = 2;
	}

	while ((scanMode == 1 && (dirEntry = readdir(toScan)) != NULL) || (scanMode == 2 && rootListItr != nullptr && rootListEntry != nullptr))
	{
		std::string entName;

		if (scanMode == 1) entName = dirEntry->d_name;
		else if (scanMode == 2)
		{
			entName = rootListEntry->GetName();

			bool skipEntry = false;
			if (acceptedClasses.size() > 0)
			{
				string entryClassName = (string) rootListEntry->ClassName();

				if (entryClassName == "TKey") entryClassName = gDirectory->GetKey(entName.c_str())->GetClassName();

				//                 cout << "Encounter object of type " << entryClassName << endl;

				skipEntry = std::find(acceptedClasses.begin(), acceptedClasses.end(), entryClassName) == acceptedClasses.end();
			}

			rootListEntry = rootListItr->Next();

			if (skipEntry) continue;
		}
		else
		{
			cerr << "Invalid mode specified...\n";
			fileList.clear();
			return fileList;
		}

		if (entName.empty()) continue;

		bool endOK = false;

		if (endWithVec.size() > 0)
		{
//             cout << "endWithVec not empty, using it instead of standard endWith..." << endl;
			for (unsigned int i = 0; i < endWithVec.size(); i++)
			{
				if (std::search(entName.begin(), entName.end(), endWithVec[i].begin(), endWithVec[i].end(), ignoreCharCasePred(caseSensitive))
						== entName.end() - endWithVec[i].length())
				{
					endOK = true;
					break;
				}
			}
		}
		else if (endWith.empty() || (endWith.length() == 1 && endWith[0] == entName[entName.length() - 1])
				|| (!endWith.empty()
						&& std::search(entName.begin(), entName.end(), endWith.begin(), endWith.end(), ignoreCharCasePred(caseSensitive))
								== entName.end() - endWith.length()))
		{
//             cout << "Found matching end" << endl;
			endOK = true;
		}

		if (startWith.empty()
				|| (!startWith.empty()
						&& std::search(entName.begin(), entName.end(), startWith.begin(), startWith.end(), ignoreCharCasePred(caseSensitive)) == entName.begin()))
		{
			if (endOK)
			{
				bool mustAllFlag = true, cantFlag = false, mustOneOfFlag = (mustOneOfTags.size() > 0 ? false : true);

				if (mustAllTags.size() > 0)
				{
					string::iterator prevTagPos = entName.begin();

					for (unsigned int i = 0; i < mustAllTags.size(); i++)
					{
						string::iterator startSearch = prevTagPos;

						string::iterator currTagPos = std::search(startSearch, entName.end(), mustAllTags[i].begin(), mustAllTags[i].end(),
								ignoreCharCasePred(caseSensitive));

						if (currTagPos == entName.end())
						{
							mustAllFlag = false;
							continue;
						}

						prevTagPos = currTagPos;
						prevTagPos++;
					}
				}

				if (cantTags.size() > 0)
				{
					for (unsigned int i = 0; i < cantTags.size(); i++)
					{
						if (std::search(entName.begin(), entName.end(), cantTags[i].begin(), cantTags[i].end(), ignoreCharCasePred(caseSensitive))
								!= entName.end())
						{
							cantFlag = true;
							continue;
						}
					}
				}

				if (mustOneOfTags.size() > 0)
				{
					for (unsigned int i = 0; i < mustOneOfTags.size(); i++)
					{
						if (std::search(entName.begin(), entName.end(), mustOneOfTags[i].begin(), mustOneOfTags[i].end(), ignoreCharCasePred(caseSensitive))
								!= entName.end()) mustOneOfFlag = true;
					}
				}

				if (mustAllFlag && !cantFlag && mustOneOfFlag) fileList.push_back((includePath ? (dirName + "/") : "") + entName);
			}
		}
	}

	return fileList;
}

std::vector<std::string> DecodeItemsToTreat(std::string itemsString, string mode, bool caseSensitive, bool includePath)
{
	std::size_t pathEnd = itemsString.find_last_of("/");

	std::string path = "";

	if (pathEnd != std::string::npos)
	{
		path = itemsString.substr(0, pathEnd);
		itemsString = itemsString.substr(pathEnd + 1);
	}

	std::vector<std::string> files;

	if (itemsString == "*") return GetDirContent(path, mode, "", "", "", "", "", false, includePath);

	std::size_t startPos = itemsString.find_first_not_of("*[]");
	std::size_t special = itemsString.find_first_of("*[]");

	std::vector<std::string> fNameParts;
	std::vector<std::tuple<std::size_t, char, std::size_t>> sChars;

	std::string fStartWith;
	std::string fEndWith;
	std::string fMustHaveAll;
	std::string fMustHaveOneOf;

	if (special == std::string::npos)
	{
		files = GetDirContent(path, mode, itemsString, itemsString, "", "", itemsString, caseSensitive, includePath);
	}
	else
	{
		if (startPos == 0)
		{
			fStartWith = itemsString.substr(0, special);
//             cout << "Should start with: " << fStartWith << endl;
		}

		if (special < startPos)
		{
			sChars.push_back(std::make_tuple(special, itemsString[special], special));
			special = itemsString.find_first_of("*[]", startPos);
		}

		while (special != std::string::npos)
		{
			fNameParts.push_back(itemsString.substr(startPos, special - startPos));

			//             std::cout << "new name part: " << itemsString.substr ( startPos, special - startPos ) << "\n";

			sChars.push_back(std::make_tuple(special, itemsString[special], startPos));

			startPos = itemsString.find_first_not_of("*[]", special);

			//             std::cout << "separator: " << itemsString[special] << " @ " << special << "\n";

			special = itemsString.find_first_of("*[]", special + 1);

			while (special < startPos)
			{
				sChars.push_back(std::make_tuple(special, itemsString[special], special));
				special = itemsString.find_first_of("*[]", special + 1);
			}
		}

		vector<string> leftOver;
		leftOver.clear();

		for (unsigned int i = 0; i < sChars.size(); i++)
		{
			if (std::get<1>(sChars[i]) == '*')
			{
				if (std::get<2>(sChars[i]) == std::get<0>(sChars[i])) continue;

				fMustHaveAll += itemsString.substr(std::get<2>(sChars[i]), std::get<0>(sChars[i]) - std::get<2>(sChars[i]));
				fMustHaveAll += " ";

//                 std::cout << "new must all tag: " << itemsString.substr ( std::get<2> ( sChars[i] ), std::get<0> ( sChars[i] ) - std::get<2> ( sChars[i] ) ) << "\n";
			}

			else if (std::get<1>(sChars[i]) == '[')
			{
				if (i == sChars.size() - 1 || std::get<1>(sChars[i + 1]) != ']')
				{
					std::cerr << "Failed to decode the files input...\n";

					return files;
				}
				else
				{
					std::string toDecode = itemsString.substr(std::get<0>(sChars[i]) + 1, std::get<0>(sChars[i + 1]) - std::get<0>(sChars[i]) - 1);

					std::vector<int> filesNbr = DecodeNumberString(toDecode, false);

					std::string baseStr = itemsString.substr(std::get<2>(sChars[i]), std::get<0>(sChars[i]) - std::get<2>(sChars[i]));
					std::string suffixStr = "";

					i++;

					bool isEnd = false;

					if (i == sChars.size() - 1)
					{
						if (std::get<0>(sChars[i]) + 1 != itemsString.length()) suffixStr = itemsString.substr(std::get<0>(sChars[i]) + 1);
						isEnd = true;
					}
					else if (std::get<1>(sChars[i + 1]) == '*' && std::get<0>(sChars[i + 1]) != std::get<0>(sChars[i]) + 1)
					{
						suffixStr = itemsString.substr(std::get<2>(sChars[i + 1]), std::get<0>(sChars[i + 1]) - std::get<2>(sChars[i + 1]));
						i++;
					}
					else if (std::get<1>(sChars[i + 1]) == '[')
					{
						vector<string> newLeftOvers;
						newLeftOvers.clear();

						for (unsigned int j = 0; j < filesNbr.size(); j++)
						{
							if (leftOver.size() > 0)
							{
								for (unsigned int k = 0; k < leftOver.size(); k++)
									newLeftOvers.push_back(leftOver[k] + (string) Form("%s%d", baseStr.c_str(), filesNbr[j]));
							}
							else newLeftOvers.push_back(Form("%s%d", baseStr.c_str(), filesNbr[j]));
						}

						leftOver = newLeftOvers;

						continue;
					}

					for (unsigned int j = 0; j < filesNbr.size(); j++)
					{
						if (leftOver.size() > 0)
						{
							for (unsigned int k = 0; k < leftOver.size(); k++)
							{
								fMustHaveOneOf += leftOver[k] + Form("%s%d%s", baseStr.c_str(), filesNbr[j], suffixStr.c_str());
								fMustHaveOneOf += " ";

//                                 std::cout << "new must one of tag: " << leftOver[k] + Form ( "%s%d%s", baseStr.c_str(), filesNbr[j], suffixStr.c_str() ) << "\n";
							}
						}
						else
						{
							fMustHaveOneOf += Form("%s%d%s", baseStr.c_str(), filesNbr[j], suffixStr.c_str());
							fMustHaveOneOf += " ";

//                             std::cout << "new must one of tag: " << Form ( "%s%d%s", baseStr.c_str(), filesNbr[j], suffixStr.c_str() ) << "\n";
						}
					}

					if (isEnd) fMustHaveOneOf += "\\shouldfinishwith";

					leftOver.clear();
				}
			}

			else if (std::get<1>(sChars[i]) == ']')
			{
				std::cerr << "Failed to decode the files input...\n";

				return files;
			}
		}

		if (startPos != std::string::npos)
		{
			fNameParts.push_back(itemsString.substr(startPos, itemsString.length()));

			fEndWith += itemsString.substr(startPos, itemsString.length());

//             cout << "Should end with: " << fEndWith << endl;
		}

		files = GetDirContent(path, mode, fEndWith, fMustHaveAll, "", fMustHaveOneOf, fStartWith, caseSensitive, includePath);
	}

	return files;
}

vector<string> SplitString(string toSplit, string splitter)
{
	std::size_t startPos = 0;
	std::size_t comaPos = toSplit.find(splitter.c_str());

	vector<string> splitString;
	splitString.clear();

	while (comaPos != string::npos)
	{
		splitString.push_back(toSplit.substr(startPos, comaPos - startPos));

		startPos = comaPos + splitter.length();
		comaPos = toSplit.find(splitter.c_str(), startPos);
	}

	splitString.push_back(toSplit.substr(startPos));

	return splitString;
}

void ReadDetectorID(std::string DetectorID, bool* isUpstream, unsigned short* numsector, bool* isBarrel, int* layernum, bool* side, bool* isSX3)
{
	std::string str(DetectorID);
	std::string IDtype;
	std::stringstream ss(str);

	std::vector<std::string> ID;

	while (ss >> IDtype)
		ID.push_back(IDtype);

	std::string UorD = ID[1].substr(0, 1);
	std::string sector = ID[1].substr(1);
	std::string defaultTreeName1;
	std::string det = ID[0].substr(0);
	std::cout << "Entered:" << std::endl;
	for (unsigned int l = 0; l < ID.size(); l++)
	{
		std::cout << ' ' << ID.at(l);
		std::cout << '\n';
	}

	if (ID.size() == 2)

	{
		if (det == "BB10")
		{

			*side = true;
			*isBarrel = true;
			*layernum = 1;
			*numsector = stoi(sector);
			*isUpstream = false;
			*isSX3 = false;

		}

		else if (det == "SX3")
		{
			*side = true;
			*numsector = stoi(sector);
			*layernum = 2;
			*isBarrel = true;
			*isSX3 = true;
			if (UorD == "U")
			{
				*isUpstream = true;
				std::cout << "Upstream ";
			}
			else
			{
				*isUpstream = false;
				std::cout << "Downstream ";
			}
		}

		else
		{
			return;
		}
	}

	else if (ID.size() == 4)
	{
		std::cout << "Entered:" << std::endl;
		for (unsigned int l = 0; l < ID.size(); l++)
		{
			std::cout << ' ' << ID.at(l);
			std::cout << '\n';
		}
		//         int ulength = ID[1].length();
		std::string upstream = ID[1];

		std::string detlayer = ID[2].substr(0);
		std::string detside = ID[3].substr(0);

		*isUpstream = (UorD == "U" ? true : false);

		if (sector == "A") *numsector = 0;
		else if (sector == "B") *numsector = 1;
		else if (sector == "C") *numsector = 2;
		else if (sector == "D") *numsector = 3;
		else *numsector = stoi(sector);

		*isSX3 = false;
		*isBarrel = true;

		if (det == "SX3" || det == "sx3" || det == "superX3" || det == "SuperX3" || det == "SUPERX3") *isSX3 = true;
		else if (det == "BB10" || det == "bb10") *isSX3 = false;
		else if (det == "QQQ5" || det == "qqq5") *isBarrel = false;
		else
		{
			std::cout << "Unknown detector named" << std::endl;
			return;
		}

		if (ID[2] == "dE") *layernum = 1;
		else if (ID[2] == "dE") *layernum = 2;
		else if (ID[2] == "dE") *layernum = 3;

		if (detside == "p") *side = true;
		else if (detside == "n") *side = false;
	}

	else
	{
		std::cout << "Incorrect Input for:" << DetectorID << std::endl;
	}
}

string GetDetectorID(bool isUpstream, bool isBarrel, unsigned int sector)
{
	string detID = Form("%s %s%d", (isBarrel ? "SuperX3" : "QQQ5"), (isUpstream ? "U" : "D"), sector);

	return detID;
}

string GetNameCompliantStr(int input)
{
	char* output = new char[128];

	if (input >= 0) sprintf(output, "%d", input);
	else sprintf(output, "m%d", -input);

	return (string) output;
}

std::function<bool(double, double)> CheckValProxFunc(double compVal)
{
	return [compVal] ( double a, double b )
	{
		bool res = fabs ( a - b ) <= compVal;
		return res;
	};
}

int ToStripID(bool isUpstream_, bool isBarrel_, bool isFront_, int sector_, int strip_)
{
	// QQQ5 UA: front = [0-31] ; back = [128-131]   --------- QQQ5 DA: front = [240-271] ; back = [368-371]
	// QQQ5 UB: front = [32-63] ; back = [132-135]  --------- QQQ5 DB: front = [272-303] ; back = [372-375]
	// QQQ5 UC: front = [64-95] ; back = [136-139]  --------- QQQ5 DC: front = [304-335] ; back = [376-379]
	// QQQ5 UD: front = [96-127] ; back = [140-143] --------- QQQ5 DD: front = [336-367] ; back = [380-383]
	// SX3 U0: front = [144-147] ; back = [192-195] --------- SX3 D0: front = [384-387] ; back = [432-435]
	//...

	int stripID = strip_;

	if (!isBarrel_)
	{
		if (isFront_) stripID += sector_ * 32;
		else stripID += 128 + sector_ * 4;
	}
	else
	{
		stripID += 144;

		if (isFront_) stripID += sector_ * 4;
		else stripID += 48 + sector_ * 4;
	}

	if (!isUpstream_) stripID += 240;

	return stripID;
}

vector<int> ToStripID(string sectorStr, bool displayList)
{
	bool isUpstream_ = true;
	bool isBarrel_ = false;
	bool isFront_ = true;

	goto tryDecode;

	invalidStr:

	std::cerr << "Invalid Sector String entered...\n";
	std::cerr << "Examples of valid Sector String:\n";
	std::cerr << "\"QQQ5 U[0-3] front [0-31]\"\n";
	std::cerr << "\"QQQ5 D[A,C] back [0-2,4]\"\n";
	std::cerr << "\"SX3 U[0,7-11] front [0-4]\"\n";
	return
	{};

	tryDecode:

	// --------------------------------------------------------------------- //

	std::size_t foundType = sectorStr.find("QQQ5");

	if (foundType == string::npos)
	{
		foundType = sectorStr.find("SX3");
		if (foundType == string::npos) goto invalidStr;

		isBarrel_ = true;
	}
	else if (sectorStr.find("SX3") != string::npos) goto invalidStr;

	// --------------------------------------------------------------------- //

	std::size_t foundSide = sectorStr.find("front");

	if (foundSide == string::npos)
	{
		foundSide = sectorStr.find("back");
		if (foundSide == string::npos) goto invalidStr;

		isFront_ = false;
	}
	else if (sectorStr.find("back") != string::npos) goto invalidStr;

	// --------------------------------------------------------------------- //

	std::size_t foundUp = sectorStr.find("U");

	if (foundUp == string::npos)
	{
		foundUp = sectorStr.find("D");
		if (foundUp == string::npos) goto invalidStr;

		isUpstream_ = false;
	}
	else if (sectorStr.find("D") != string::npos) goto invalidStr;

	// --------------------------------------------------------------------- //

	vector<int> sectorsList;

	std::size_t openBracket = sectorStr.find_first_of("[");
	std::size_t closeBracket = sectorStr.find_first_of("]", openBracket + 1);

	if (openBracket > foundSide || (openBracket == string::npos && closeBracket == string::npos))
	{
		string sectorNumStr = sectorStr.substr(foundUp + 1, foundSide - foundUp - 1);
		sectorNumStr = FindAndReplaceInString(sectorNumStr, " ", "");

		//         cout << "Sectors Number String: " << sectorNumStr << endl;

		if (sectorNumStr.find_first_not_of("0123456789") != string::npos) goto invalidStr;

		sectorsList.push_back(std::stoi(sectorNumStr));
	}
	else if ((openBracket != string::npos && closeBracket == string::npos) || (openBracket == string::npos && closeBracket != string::npos)) goto invalidStr;
	else if (openBracket != string::npos)
	{
		string sectorNumStr = sectorStr.substr(openBracket + 1, closeBracket - openBracket - 1);
		sectorNumStr = FindAndReplaceInString(sectorNumStr, " ", "");

		//         cout << "Sectors Number String: " << sectorNumStr << endl;

		if (sectorNumStr.find_first_not_of("0123456789,-") != string::npos) goto invalidStr;

		sectorsList = DecodeNumberString(sectorNumStr);
	}

	// --------------------------------------------------------------------- //

	vector<int> stripsList;

	openBracket = sectorStr.find_first_of("[", foundSide);
	closeBracket = sectorStr.find_first_of("]", openBracket + 1);

	if ((openBracket != string::npos && closeBracket == string::npos) || (openBracket == string::npos && closeBracket != string::npos)) goto invalidStr;
	else if (openBracket != string::npos)
	{
		string stripNumStr = sectorStr.substr(openBracket + 1, closeBracket - openBracket - 1);
		stripNumStr = FindAndReplaceInString(stripNumStr, " ", "");

		//         cout << "Strips Number String: " << stripNumStr << std::endl;

		if (stripNumStr.find_first_not_of("0123456789,-") != string::npos) goto invalidStr;

		stripsList = DecodeNumberString(stripNumStr);
	}
	else
	{
		size_t foundSpace = sectorStr.find_first_of(" ", foundSide + 1);
		size_t foundEnd = sectorStr.find_first_of(" \0", foundSide + 1);

		string stripNumStr = sectorStr.substr(foundSpace + 1, foundEnd - foundSpace - 1);
		stripNumStr = FindAndReplaceInString(stripNumStr, " ", "");

		//         cout << "Strips Number String: " << stripNumStr << endl;

		if (stripNumStr.find_first_not_of("0123456789") != string::npos) goto invalidStr;

		stripsList.push_back(std::stoi(stripNumStr));
	}

	vector<int> stripIDsList;

	if (displayList) cout << sectorStr << " correspond to strips ID:\n";

	for (unsigned int sectI = 0; sectI < sectorsList.size(); sectI++)
	{
		for (unsigned int stripI = 0; stripI < stripsList.size(); stripI++)
		{
			int sid = ToStripID(isUpstream_, isBarrel_, isFront_, sectorsList[sectI], stripsList[stripI]);

			if (displayList) cout << " * " << sid << endl;

			stripIDsList.push_back(sid);
		}
	}

	return stripIDsList;
}

void ToStripID(vector<int>* dest, string sectorStr)
{
	if (dest->size() == 0) dest->clear();

	vector<int> toAdd = ToStripID(sectorStr);

	dest->insert(dest->end(), toAdd.begin(), toAdd.end());
}

string FromStripID(int stripID_, bool& isUpstream_, bool& isBarrel_, bool& isFront_, int& sector_, int& strip_)
{
	// QQQ5 UA: front = [0-31] ; back = [128-131]   --------- QQQ5 DA: front = [240-271] ; back = [368-371]
	// QQQ5 UB: front = [32-63] ; back = [132-135]  --------- QQQ5 DB: front = [272-303] ; back = [372-375]
	// QQQ5 UC: front = [64-95] ; back = [136-139]  --------- QQQ5 DC: front = [304-335] ; back = [376-379]
	// QQQ5 UD: front = [96-127] ; back = [140-143] --------- QQQ5 DD: front = [336-367] ; back = [380-383]
	// SX3 U0: front = [144-147] ; back = [192-195] --------- SX3 D0: front = [384-387] ; back = [432-435]
	//...

	bool up, bar, front;
	int sect, stri;

	if (stripID_ >= 240)
	{
		up = false;
		stripID_ -= 240;
	}
	else up = true;

	if (stripID_ >= 144)
	{
		bar = true;

		stripID_ -= 144;

		if (stripID_ >= 48)
		{
			front = false;
			stripID_ -= 48;
		}
		else front = true;

		sect = (int) (stripID_ / 4);
		stri = stripID_ % 4;
	}
	else
	{
		bar = false;

		if (stripID_ >= 128)
		{
			front = false;
			stripID_ -= 128;

			sect = (int) (stripID_ / 4);
			stri = stripID_ % 4;
		}
		else
		{
			front = true;
			sect = (int) (stripID_ / 32);
			stri = stripID_ % 32;
		}
	}

	char* sectorStr = new char[512];

	isUpstream_ = up;
	isBarrel_ = bar;
	isFront_ = front;
	sector_ = sect;
	strip_ = stri;

	sprintf(sectorStr, "%s %s%d %s %d", (bar ? "SX3" : "QQQ5"), (up ? "U" : "D"), sect, (front ? "front" : "back"), stri);

	return (string) sectorStr;
}

string FromStripID(int stripID_)
{
	bool dummy1, dummy2, dummy3;
	int dummy4, dummy5;

	return FromStripID(stripID_, dummy1, dummy2, dummy3, dummy4, dummy5);
}

//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

int RoundValue(double val)
{
	int floorVal = (int) val;

	double decimals = (val - floorVal) * 10;
	int firstDecimal = (int) decimals;

	if (firstDecimal >= 5) return ceil(val);
	else return floor(val);
}

std::pair<vector<double>, vector<double> > FillGraphFromFile(string input)
{
	vector<double> xs, ys;
	xs.clear();
	ys.clear();

	std::ifstream inFile(input.c_str(), std::ios_base::in);

	if (!inFile.is_open())
	{
		std::cerr << "Failed to open file " << input << std::endl;
		return std::make_pair(xs, ys);
	}

	string readLine;

	while (std::getline(inFile, readLine))
	{
		std::istringstream iss;
		iss.str(readLine);

		double x_, y_;

		iss >> x_ >> y_;

		xs.push_back(x_);
		ys.push_back(y_);
	}

	inFile.close();

	return std::make_pair(xs, ys);
}

double EvalGraph(vector<double> x_, vector<double> y_, double toEval)
{
	if (x_.size() == 0 || y_.size() == 0) return 0.;

	auto lowItr = std::find(x_.begin(), x_.end(), toEval);

	auto lstItr = x_.end();
	lstItr--;

	int low, up;

	low = std::distance(x_.begin(), lowItr);

	if (lowItr != x_.end()) return y_[low];
	else
	{
		lowItr = std::lower_bound(x_.begin(), x_.end(), toEval);

		if (lowItr == x_.end()) lowItr--;

		if (lowItr != x_.begin()) lowItr--;

		low = std::distance(x_.begin(), lowItr);

		auto upItr = lowItr;
		upItr++;

		up = std::distance(x_.begin(), upItr);

		return (y_[up] + (toEval - x_[up]) * (y_[low] - y_[up]) / (x_[low] - x_[up]));
	}

	return 0.;
}

double IntegrateGraph(vector<double> x_, vector<double> y_, double xMin_, double xMax_, double dx_)
{
	if (xMin_ >= xMax_) return 0.0;

	double yMin = EvalGraph(x_, y_, xMin_);
	double yMax = EvalGraph(x_, y_, xMax_);

	double prevY = yMin;
	double nextX = xMin_ + dx_;

	double totIntegral = 0.0;

	while (nextX < xMax_)
	{
		double nextY = EvalGraph(x_, y_, nextX);

		totIntegral += nextY * dx_ + (prevY - nextY) * dx_ / 2.;

		nextX += dx_;
		prevY = nextY;
	}

	totIntegral += yMax * (xMax_ - nextX) + (prevY - yMax) * (xMax_ - nextX) / 2.;

	return totIntegral;
}

double GetEffectiveThickness(double angle, double targetThickness_)
{
	return fabs(targetThickness_ / (2 * TMath::Cos(angle)));
}

double ComputeEnergyLoss(vector<double> energies_, vector<double> rangeOrStoppingPower_, double startingEnergy, float mass, double xMin_, double xMax_,
		double dx_, string mode)
{
	if (xMin_ >= xMax_) return 0.0;

	if (mode == "Integral")
	{
		double nextX = xMin_;

		double remainingEnergy = startingEnergy;

		while (nextX < xMax_)
		{
			remainingEnergy -= EvalGraph(energies_, rangeOrStoppingPower_, remainingEnergy) * dx_ / mass;

			nextX += dx_;
		}

		remainingEnergy -= EvalGraph(energies_, rangeOrStoppingPower_, remainingEnergy) * (xMax_ - nextX) / mass;

		return (startingEnergy - remainingEnergy) * mass;
	}
	else if (mode == "Interpolation")
	{
		double rangeMax = EvalGraph(energies_, rangeOrStoppingPower_, startingEnergy);

		double remainingEnergy = EvalGraph(rangeOrStoppingPower_, energies_, rangeMax - (xMax_ - xMin_));

		return (startingEnergy - remainingEnergy) * mass;
	}
	else return 0.0;
}

double TryGetRemainingEnergy(string mass_db, int mass, int charge, double startingEnergy, double thickness_, double dx_, string targetStr, double density,
		string tablePath, string mode)
{
	std::ifstream mass_input(mass_db.c_str(), std::ios_base::in);

	if (!mass_input.is_open())
	{
		std::cerr << "Failed to open the mass database. Energy loss has not been computed...\n";

		return startingEnergy;
	}

	string elementStr = "";

	GetAtomicFormula(mass_input, mass, charge, elementStr);

	char* tryFindStr = new char[512];

	char* densityStr = new char[128];

	if (density == 0) sprintf(densityStr, "*");
	else sprintf(densityStr, "%d", (int) density);

	sprintf(tryFindStr, "%s/%s_in_%s_%smg_cm3*", tablePath.c_str(), elementStr.c_str(), targetStr.c_str(), densityStr);

	vector<string> tryFindTable = DecodeItemsToTreat((string) tryFindStr, "system", false);

	double remainingEnergy = startingEnergy;

	if (tryFindTable.size() == 1)
	{
		auto beamTable = FillGraphFromFile(tablePath + "/" + tryFindTable[0]);

		double eLoss = ComputeEnergyLoss(beamTable.first, beamTable.second, startingEnergy / mass, mass, 0, thickness_, dx_, mode);

		std::cout << "Computed energy loss using table " << tryFindTable[0] << " : " << eLoss << " MeV" << std::endl;

		remainingEnergy -= eLoss;
	}
	else
	{
		cerr << "Could not compute beam energy loss. Found " << tryFindTable.size() << " match for the energy loss table ( " << tryFindStr << " )" << endl;

		for (unsigned int i = 0; i < tryFindTable.size(); i++)
			cerr << "---> " << tryFindTable[i] << endl;

		cerr << endl;
	}

	return remainingEnergy; // in MeV
}

void ListHistograms(string match, unsigned int limit, unsigned int startAt, bool caseSensitive)
{
	std::vector<string> matchingHists;
	matchingHists.clear();

	if (match.empty()) match = "*";

	matchingHists = DecodeItemsToTreat(match, "root TH1F TH2F TH1D TH2D", caseSensitive);

	if (limit == 0) limit = 20;

	unsigned int histAmount = gDirectory->GetList()->GetSize();

	unsigned int digitsNumber = 1;

	string userIn;

	while ((histAmount / pow(10, digitsNumber)) >= 1)
		digitsNumber++;

	if (startAt >= matchingHists.size())
	{
		return;
	}

	cout << matchingHists.size() << " matching histograms...\n\n";

	DisplayResults:

	for (unsigned int i = startAt; i < std::min((unsigned int) matchingHists.size(), startAt + limit); i++)
	{
		cout << "-> Histogram #" << std::setw(digitsNumber) << i << " : " << matchingHists[i] << endl;
	}

	startAt += limit;

	if (startAt >= matchingHists.size())
	{
		return;
	}

	UserPrompt:

	cout << "Continue = Enter / Quit = q : ";

	std::getline(std::cin, userIn);

	if (userIn == "q" || userIn == "quit") return;
	else if (userIn.empty()) goto DisplayResults;
	else
	{
		cerr << "Invalid input...\n";
		goto UserPrompt;
	}

	return;
}

void WriteHistograms(string outfile, string match, bool caseSensitive)
{
	std::vector<string> matchingHists;
	matchingHists.clear();

	if (match.empty()) match = "*";

	matchingHists = DecodeItemsToTreat(match, "root TH1F TH2F TH1D TH2D", caseSensitive);

	cout << matchingHists.size() << " matching histograms...\n\n";

	if (matchingHists.size() > 0)
	{
		string histsDir = gDirectory->GetPath();

		TFile* f = new TFile(outfile.c_str(), "recreate");

		for (unsigned int i = 0; i < matchingHists.size(); i++)
		{
			cout << "Written " << i << " / " << matchingHists.size() << " histograms...\r" << flush;
			gDirectory->cd(histsDir.c_str());
			auto obj = gDirectory->Get(matchingHists[i].c_str());

			f->cd();

			if (obj != nullptr)
			{
				if (((string) obj->ClassName()).find("TH2") != string::npos)
				{
					((TH2F*) obj)->Write();
				}
				else if (((string) obj->ClassName()).find("TH1") != string::npos)
				{
					((TH1F*) obj)->Write();
				}
			}
		}
		cout << endl;

		f->Close();
	}

	return;
}

//____________________________________________________________________________________________//
//____________________________________________________________________________________________//

int InitReadMassesForKinematic(std::ifstream& mass_db)
{
	if (!mass_db.is_open())
	{
		std::cerr << "No File Found for the Masses Database!\n";
		return -1;
	}

	std::stringstream readMassDB;
	string readLine;

	int massTableLineBeg = -1;

	while (std::getline(mass_db, readLine))
	{
		if (readLine.length() == 0) continue;

		readMassDB.clear();
		readMassDB.str(readLine);

		if (readLine.find("MASS") != string::npos && readLine.find("LIST") != string::npos)
		{
			massTableLineBeg += 5;
			break;
		}
	}

	return massTableLineBeg;
}

void GetRelevantInfoPositions(string* readWord, short& posMassExcess, short& posBindingEnergy, short& posBetaDecay, short& posAMU, short& posElement)
{
	std::stringstream checkNumConversion;
	float dummyf;

	checkNumConversion.str(readWord[posElement]);
	checkNumConversion >> dummyf;

	if (!checkNumConversion.fail()) // Our element is a number and not a string!
	{
		//         std::cout << Form ( "Did not fail to parse Word#%d as a float ** ", posElement );
		posMassExcess++;
		posElement++;
	}

	checkNumConversion.clear();
	checkNumConversion.str(readWord[posMassExcess]);
	checkNumConversion >> dummyf;

	if (checkNumConversion.fail()) // Our Mass Excess number is not a number!
	{
		//         std::cout << Form ( "Failed to parse Word#%d as an float ** ", posMassExcess );
		posMassExcess++;
	}

	posBindingEnergy = posMassExcess + 2;

	posBetaDecay = posBindingEnergy + 3;

	if (readWord[posMassExcess][readWord[posMassExcess].length() - 1] == '#') posMassExcess = -1;

	if (readWord[posBindingEnergy][readWord[posBindingEnergy].length() - 1] == '#') posBindingEnergy = -1;

	if (readWord[posBetaDecay] == "*")
	{
		posAMU = posBetaDecay + 1;
		posBetaDecay = -1;
	}
	else
	{
		posAMU = posBetaDecay + 2;
	}

	if (readWord[posAMU + 1][readWord[posAMU + 1].length() - 1] == '#') posAMU = -1;

	return;
}

void GetAtomicFormula(std::ifstream& mass_db, int mass, int charge, string& toReconstruct)
{
	mass_db.clear();
	mass_db.seekg(0);

	string element = "";

	if (mass == 1 && charge == 1)
	{
		toReconstruct = "p";
	}
	else if (mass == 2 && charge == 1)
	{
		toReconstruct = "d";
	}
	else if (mass == 1 && charge == 0)
	{
		toReconstruct = "n";
	}
	else if (mass == 3 && charge == 1)
	{
		toReconstruct = "t";
	}
	else
	{
		std::stringstream readMassDB;
		string readLine;

		int massTableLineBeg = InitReadMassesForKinematic(mass_db);

		for (int i = 0; i < massTableLineBeg; i++)
		{
			std::getline(mass_db, readLine);
		}

		while (std::getline(mass_db, readLine))
		{
			readMassDB.clear();
			readMassDB.str(readLine);

			short posMassExcess = 5, posBindingEnergy = -1, posBetaDecay = -1, posAMU = -1, posElement = 4;

			string readWord[17];

			//         std::cout << "Read Line:\n";

			for (int i = 0; i < 17; i++)
			{
				readMassDB >> readWord[i];
				//             std::cout << readWord[i] << "  ";
			}

			//         std::cout << "\n";

			GetRelevantInfoPositions(readWord, posMassExcess, posBindingEnergy, posBetaDecay, posAMU, posElement);

			if (charge == std::stoi(readWord[posElement - 2]))
			{
				char* massChar = new char[4];
				sprintf(massChar, "%i", mass);

				toReconstruct = massChar;
				toReconstruct += readWord[posElement];

				break;
			}
		}

		return;
	}
}

bool CharIsDigit(char toCheck)
{
	if (toCheck == '0' || toCheck == '1' || toCheck == '2' || toCheck == '3' || toCheck == '4' || toCheck == '5' || toCheck == '6' || toCheck == '7'
			|| toCheck == '8' || toCheck == '9') return true;
	else return false;
}

void DecodeAtomicFormula(std::ifstream& mass_db, string toDecode, int& mass, int& charge, float& atomicMass)
{
	mass_db.clear();
	mass_db.seekg(0);

	std::vector<int> massDigits;

	string element = "";
	mass = 0;
	charge = -1;

	massDigits.clear();

	if (toDecode == "p")
	{
		mass = 1;
		element = "H";
	}
	else if (toDecode == "d")
	{
		mass = 2;
		element = "H";
	}
	else if (toDecode == "n")
	{
		mass = 1;
		element = "n";
	}
	else if (toDecode == "t")
	{
		mass = 3;
		element = "H";
	}
	else
	{
		for (unsigned int i = 0; i < toDecode.length(); i++)
		{
			if (CharIsDigit(toDecode[i]))
			{
				string digit;
				digit = toDecode[i];

				massDigits.push_back(std::stoi(digit));
			}
			else
			{
				element += toDecode[i];
			}
		}

		for (unsigned short i = 0; i < massDigits.size(); i++)
		{
			//         std::cout << "Retreived mass digit: " << massDigits[i] << "\n";
			mass += massDigits[i] * pow(10, massDigits.size() - 1 - i);
		}
	}

	//     std::cout << "Retreiving the charge of element: " << element << " ...\n";
	//     std::cout << "Decoded Mass: " << mass << "\n";

	std::stringstream readMassDB;
	string readLine;

	int massTableLineBeg = InitReadMassesForKinematic(mass_db);

	for (int i = 0; i < massTableLineBeg; i++)
	{
		std::getline(mass_db, readLine);
	}

	short posMassExcess = 5, posBindingEnergy = -1, posBetaDecay = -1, posAMU = -1, posElement = 4;

	while (std::getline(mass_db, readLine))
	{
		readMassDB.clear();
		readMassDB.str(readLine);

		string readWord[17];

		//         std::cout << "Read Line:\n";

		for (int i = 0; i < 17; i++)
		{
			readMassDB >> readWord[i];
			//             std::cout << readWord[i] << "  ";
		}

		//         std::cout << "\n";

		posMassExcess = 5;
		posBindingEnergy = -1;
		posBetaDecay = -1;
		posAMU = -1;
		posElement = 4;

		GetRelevantInfoPositions(readWord, posMassExcess, posBindingEnergy, posBetaDecay, posAMU, posElement);

		charge = CheckForMatch<string>(readWord, posElement, mass, element);

		if (charge >= 0) break;
	}

	readMassDB.clear();
	readMassDB.str(readLine);

	string mainAMStr;
	string secondaryAMStr;

	for (short i = 0; i < posAMU + 1; i++)
		readMassDB >> mainAMStr;
	readMassDB >> secondaryAMStr;

	string atomicMassStr = mainAMStr + secondaryAMStr;

	//     std::size_t fstSpacePos = readLine.find_first_of ( " ", posAMU );;
	//     std::size_t amEndPos = readLine.find_first_of ( " \n", fstSpacePos+1 );
	//     atomicMassStr = readLine.substr ( posAMU, amEndPos-posAMU );
	//
	//     atomicMassStr = FindAndReplaceInString ( atomicMassStr, " ", "" );

	//     std::cout << "Atomic Mass String: " << atomicMassStr << std::endl;

	atomicMass = std::stof(atomicMassStr);
	atomicMass *= 1e-6;

	return;
}

//_______________________________________________________________________________________________________________________________________________//
//_________________________________________GEOMETRY AND TRIGONOMETRY FUNCTIONS & UTILITIES_______________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

float findPolarFromCartesian(float xx, float yy, float zz, float* rr)
{
	float d1;

	*rr = sqrtf(xx * xx + yy * yy + zz * zz);
	d1 = acosf(zz / *rr);

	return d1;
}

float findAzimuthFromCartesian(float xx, float yy)
{
	float d1;

	d1 = atan2f(yy, xx);

	return d1;
}

int GetASeed(unsigned int *seed)
{
	/* delarations */

	struct timeval tp;
	int i;
	unsigned short int i1;

	/* make the random seed */

	for (i = 0; i < 31; i++)
	{
		gettimeofday(&tp, NULL);
		i1 = (unsigned int) tp.tv_usec;
		i1 &= 0x0001;
		*seed += i1 << i;
	};
	/* printf("GetASeed:: %i\n", *seed); */

	return 0;
}

//_______________________________________________________________________________________________________________________________________________//
//_____________________________________________________BINARY FILES UTILITIES____________________________________________________________________//
//_______________________________________________________________________________________________________________________________________________//

ifstream binaryFileToRead;
string currentBinaryFileName;
std::pair<BinDecHeadStruct*, char*>* gebWord = 0;
char* binPayload;

unsigned long long int BytesCounter(string fName)
{
	ifstream binFile(fName.c_str(), std::ios::in | std::ios::binary);

	unsigned long long int nBytes = 0;

	if (!binFile.is_open()) return nBytes;

	binFile.seekg(0, binFile.end);

	nBytes = binFile.tellg();

	binFile.close();

	return nBytes;
}

bool OpenBinaryFile(string fname)
{
	cout << "Opening file " << fname << "\n";

	if (binaryFileToRead.is_open())
	{
		binaryFileToRead.close();
	}

	binaryFileToRead.open(fname.c_str(), std::ios::in | std::ios::binary);
	currentBinaryFileName = fname;

	if (!binaryFileToRead.is_open())
	{
		cerr << "Failed to open file: " << fname << "\n";

		return false;
	}

	return true;
}

char* ReadBinData(int size)
{
	char* binRead = new char[size];
	binaryFileToRead.read(binRead, size);

	return binRead;
}

void InvertEndianness(char** word, int sizeofword)
{
	unsigned long long int* tempData = (unsigned long long int*) (*word);
	unsigned long long int* finalData = new unsigned long long int();
	*finalData = *tempData;

//         cout << "Byte swapping...\n";

	int nbytes = sizeofword;
	*finalData = 0;

	for (int i = 0; i < nbytes; i++)
	{
		long long unsigned int mask = 0;
		for (int j = i * 8; j < i * 8 + 8; j++)
			mask += (long long unsigned int) pow(2, j);

		if (i < nbytes / 2) *finalData += (*(tempData) & mask) << (8 * (nbytes - i * 2 - 1));
		else *finalData += (*(tempData) & mask) >> (8 * abs(nbytes - i * 2 - 1));
	}

//         cout << "Byte swapping done...\n";

	*word = (char*) finalData;
}

char* GetAWord(ifstream& inFile, int nBytesToRead, bool invertWords)
{
	if (binPayload == NULL)
	{
		binPayload = new char[50000];
		//         cout << "Allocated space for the payload...\n";
	}

	string dummy;

	inFile.read(binPayload, nBytesToRead);

	unsigned int* relevantData = (unsigned int*) binPayload;

	if (invertWords)
	{
//         cout << "Byte swapping...\n";

		unsigned int j = 0;
		while (j < gebWord->first->length / sizeof(unsigned int))
		{
			/* before 4 3 2 1 */
			/*        | | | | */
			/* after  1 2 3 4 */

			int t1 = (*(relevantData + j) & 0x000000ff) << 24;
			int t2 = (*(relevantData + j) & 0x0000ff00) << 8;
			int t3 = (*(relevantData + j) & 0x00ff0000) >> 8;
			int t4 = (*(relevantData + j) & 0xff000000) >> 24;
			*(relevantData + j) = t1 + t2 + t3 + t4;

			j++;
		}

//         cout << "Byte swapping done...\n";
	}

	return binPayload;
}

char* GetAWord(ifstream& inFile, string binType, bool invertWords)
{
	if (binType == "GEB" && gebWord == NULL)
	{
		//         cout << "First readout... Creating objects...\n";
		gebWord = new std::pair<BinDecHeadStruct*, char*>;
		gebWord->first = new BinDecHeadStruct;
		gebWord->second = new char[50000];
		binPayload = gebWord->second;
	}

	int nBytesToRead;

	if (binType == "GEB")
	{
		//         cout << "Reding header...\n";

		inFile.read((char*) gebWord->first, sizeof(BinDecHeadStruct));

		//         cout << "Reading payload...\n";

		nBytesToRead = gebWord->first->length;
	}
	else
	{
		nBytesToRead = 8194 * 4;
	}

	return GetAWord(inFile, nBytesToRead, invertWords);
}

char* GetAWord(string binType, bool invertWords)
{
	return GetAWord(binaryFileToRead, binType, invertWords);
}

char* GetAWord(string binType, unsigned long long int eventNbr, bool invertWords)
{
	if (!binaryFileToRead.is_open())
	{
		cerr << "No binary file loaded...\n";

		return nullptr;
	}

	binaryFileToRead.seekg(0);

	if (eventNbr > 0)
	{
		for (unsigned long long int i = 0; i < eventNbr; i++)
		{
			GetAWord(binType, false);
		}
	}

	return GetAWord(binType, invertWords);
}

char* GetAWord(string fname, string binType, unsigned long long int eventNbr, bool invertWords)
{
	OpenBinaryFile(fname);

	return GetAWord(binType, eventNbr, invertWords);
}

void ReadGEBHeader()
{
	if (gebWord == NULL) return;

	cout << "---- Reading GEB Header ----\n";
	cout << "* type : " << gebWord->first->type << "\n";
	cout << "* length : " << gebWord->first->length << "\n";
	cout << "* timestamp : " << gebWord->first->timestamp << "\n";
}

bool GEBBinaryEvFilter(std::pair<BinDecHeadStruct*, char*>* readWord_)
{
	//     cout << "Entering event filter...\n";

	int type = readWord_->first->type;
	/*int length = readWord_->first->length;
	 unsigned long long int timestamp = readWord_->first->timestamp;
	 char* data = gebWord->second;*/

	if (type > 0) return true;
	else return false;
}

void DecodeGEBBinary(string fname, unsigned long int nevents, unsigned long int fstevent, bool invertWords, bool quietMode, string outRootName,
		string mapFileName)
{
	unsigned int chMap[2410];
	fill(chMap, chMap + 2410, -1);

	unsigned int chType[2410];
	fill(chType, chType + 2410, -1);

	ifstream mapping(mapFileName.c_str());

	if (!mapping.is_open())
	{
		cerr << "Failed to open the map file!...\n";

		return;
	}

	string dummy;
	unsigned int readID, readCh, readChType;

	while (!mapping.eof())
	{
		mapping >> readID >> readChType >> readCh >> dummy;

		if (readID < 2410)
		{
			chMap[readID] = readCh;
			chType[readID] = readChType;
		}
		else
		{
			cerr << "Unrecognized ID " << readID << " !...\n";

			return;
		}
	}

	cout << "Byte swapping is " << (invertWords ? "ON" : "OFF") << "...\n";

	unsigned long int i = 0;
	unsigned long int counter = 0;

	binaryFileToRead.open(fname.c_str(), std::ios::in | std::ios::binary);

	binaryFileToRead.seekg(0, binaryFileToRead.end);

	unsigned long long int binSize = binaryFileToRead.tellg();
	cout << "Binary file size: " << binSize << endl;
	unsigned long long int bytesRead = 0;

	binaryFileToRead.seekg(0, binaryFileToRead.beg);

	int type, channel;
	unsigned long int value;
	unsigned long long int timestamp;

	//     TTree* outRoot = 0;
	//     TFile* outRootFile = 0;

	if (!outRootName.empty())
	{
		cout << "Saving results to " << outRootName << " (actually not currently implemented...)" << endl;
		//         outRoot = MakeRootTree ( &type, &channel, &value, &timestamp );
		//         outRootFile = new TFile ( outRootName.c_str(), "recreate" );
	}

	while (counter < nevents && !binaryFileToRead.eof())
	{
		if (!binaryFileToRead.is_open())
		{
			cerr << "Failed to open binary file!...\n";

			return;
		}

		//         if ( ( i ) %10000 == 0 )
		//         {
		//             cout << ( i < fstevent ? "Skipping phase...\n" : "\n" );
		//             cout << "Read " << bytesRead << " bytes out of " << binSize << " (" << ( float ) bytesRead/binSize * 100. << "%)\n";
		//             cout << "Read " << i-fstevent << " events" << esc << "[1A" << "\r" << flush;
		//         }

		GetAWord("GEB", invertWords);

		bytesRead += sizeof(BinDecHeadStruct);
		bytesRead += gebWord->first->length;

		// 	cout << "Read " << bytesRead << " bytes...\n";

		type = gebWord->first->type;
		timestamp = gebWord->first->timestamp;

		// 	cout << "Type is " << type << " / timestamp is " << timestamp << "\n";

		if (i >= fstevent)
		{
			int boardID, channelNumber;
			unsigned long int preRise, postRise1, postRise2;

			boardID = ReadPayload<int>(0, 4, 15, 0, false);
			channelNumber = ReadPayload<int>(0, 0, 3, 0, false);

			postRise1 = ReadPayload<unsigned long int>(7, 24, 31, 0, false);
			postRise2 = ReadPayload<unsigned long int>(8, 0, 15, 8, false);
			preRise = ReadPayload<unsigned long int>(7, 0, 23, 0, false);

			// 	    cout << "Done reading the payload...\n";

			int channelID = boardID * 10 + channelNumber;

			// 	    cout << "Channel ID is " << channelID << "\n";

			channel = (type != 19 ? chMap[channelID] : 0);

			// 	    cout << "Reconstructed channel: " << channel << "\n";

			value = (postRise1 + postRise2 - preRise) / 10;

			bool validEvent = GEBBinaryEvFilter(gebWord);

			//             cout << "Exiting event filter...\n";

			if (validEvent)
			{
				//                 if ( outRoot != NULL )
				//                 {
				//                     if ( counter%10000 == 0 ) cout << "Treated " << counter << " events out of " << nevents << " ( " << ( float ) counter/nevents * 100. << " % )\r" << flush;
				//
				//                     outRoot->Fill();
				//                 }

				//                 if ( !quietMode && type == 14 && chType[channelID] >= 3 )
				if (!quietMode)
				{
					cout << endl;
					cout << "\n-------- Event number " << i << " --------" << endl;

					ReadGEBHeader();

					if (type == 19)
					{

					}

					else if (type == 16)
					{
						cout << std::left << std::setw(17) << "* channel ID : " << channelID << "" << endl;
						cout << std::left << std::setw(17) << "* channel : " << channel << "" << endl;
						cout << std::left << std::setw(17) << "* value : " << value << "" << endl;
						cout << "==> Dumping the raw words:" << endl;

						for (int ww = 0; ww < 16; ww++)
						{
							unsigned int res = ReadPayload<unsigned int>(ww, 0, 31, 0, false);

							cout << "   - Word " << ww << ": " << res << endl;
						}
					}

					else if (type == 14)
					{
						cout << std::left << std::setw(17) << "* channel ID : " << channelID << "" << endl;
						cout << std::left << std::setw(17) << "* channel Type : " << chType[channelID] << "" << endl;
						cout << std::left << std::setw(17) << "* channel : " << chMap[channelID] << "" << endl;
						cout << std::left << std::setw(17) << "* value : " << value << "" << endl;

						cout << "==> Dumping the raw words:" << endl;

						for (int ww = 0; ww < 16; ww++)
						{
							unsigned int res = ReadPayload<unsigned int>(ww, 0, 31, 0, false);

							cout << "   - Word " << ww << ": " << res << endl;
						}
					}
				}

				counter++;
			}
		}

		i++;
	}

	cout << "\n\n\n";

	//     if ( outRootFile != NULL && outRootFile->IsOpen() )
	//     {
	//         outRootFile->cd();
	//
	//         outRoot->Write();
	//
	//         outRootFile->Close();
	//     }

	binaryFileToRead.close();
	binaryFileToRead.clear();

	return;
}

TGraph* GetGEBWaveForm()
{
	int np = ReadPayload<int>(0, 16, 26, 0, false);

	TGraph* wave = new TGraph((np - 13) * 2);

	int p = 0;

	for (int i = 13; i < np; i++)
	{
		unsigned int amp = ReadPayload<unsigned int>(i, 0, 13, 0, false);
		wave->SetPoint(p, p, amp);
		p++;

		amp = ReadPayload<unsigned int>(i, 16, 29, 0, false);
		wave->SetPoint(p, p, amp);
		p++;
	}

	wave->Draw("ACP");

	return wave;
}

// void InitializePlotGEBWaveForm ( string fname, string binType, bool invertWords )
// {
//     GetAWord ( fname, binType, 0, invertWords );
// }

// TH2F* PlotGEBWaveForm ( std::function<bool() > channelFilterFn, long long int reqN, long long int maxEv, bool invertWords )
// {
//     TH2F* res = new TH2F ( "Wave_Form", "Wave Form", 1024, 0, 1024, 9000, 0, 18000 );
//
//     int matching = 0;
//     int processed = 0;
//
//     while ( matching < reqN && processed != maxEv && !binaryFileToRead.eof() )
//     {
//         if ( processed%100 == 0 ) cout << "Processed " << processed << " event ( found " << matching << " matching)\r" << flush;
//
//         if ( channelFilterFn() )
//         {
//             int np = ReadPayload<int> ( 0, 16, 26, 0, false );
//
//             int sample = 0;
//
//             for ( int i = 13; i < np; i++ )
//             {
//                 unsigned int amp = ReadPayload<unsigned int> ( i, 0, 13, 0, false );
//                 res->Fill ( sample, amp );
//                 sample++;
//
//                 amp = ReadPayload<unsigned int> ( i, 16, 29, 0, false );
//                 res->Fill ( sample, amp );
//                 sample++;
//             }
//
//             matching++;
//         }
//
//         processed++;
//
//         GetAWord ( "GEB", invertWords );
//     }
//
//     cout << endl;
//     cout << "Done processing " << processed << " event ( found " << matching << " matching)\r" << endl;
//
//     res->Draw ( "colz" );
//
//     return res;
// }

// TH2F* PlotGEBWaveForm ( string channelFilterFn, long long int reqN, long long int maxEv, bool invertWords, bool init )
// {
//     LoadLuaFile ( channelFilterFn );
//
//     if ( lua_type ( lua, -1 ) != LUA_TFUNCTION || lua_gettop ( lua ) < 2 )
//     {
//         cerr << "lua file provided has an invalid format!" << endl;
//         return nullptr;
//     }
//
//     vector<string> binFiles;
//
//     if ( lua_type ( lua, -2 ) == LUA_TTABLE )
//     {
//         lua_pushnil ( lua );
//
//         while ( lua_next ( lua, -3 ) != 0 )
//         {
//             if ( lua_type ( lua, -1 ) != LUA_TSTRING )
//             {
//                 cerr << "Invalid return for the first element of the lua script" << endl;
//                 return nullptr;
//             }
//
//             cout << "Adding file to treat: " << lua_tostring ( lua, -1 ) << endl;
//             binFiles.push_back ( lua_tostring ( lua, -1 ) );
//
//             lua_pop ( lua, 1 );
//         }
//     }
//
//     cout << "stack size: " << lua_gettop ( lua ) << endl;
//
//     TH2F* res = new TH2F ( "Wave_Form", "Wave Form", 1024, 0, 1024, 9000, 0, 18000 );
//
//     int matching = 0;
//     int processed = 0;
//
//     int fnum = 0;
//
//     if ( init ) InitializePlotGEBWaveForm ( binFiles[fnum], "GEB", true );
//     else
//     {
//         for ( unsigned int i = 0; i < binFiles.size(); i++ )
//         {
//             if ( currentBinaryFileName == binFiles[i] ) break;
//
//             fnum++;
//         }
//
//         GetAWord ( "GEB", invertWords );
//     }
//
//     while ( matching < reqN && processed != maxEv && !binaryFileToRead.eof() && fnum < binFiles.size() )
//     {
//         if ( processed%100 == 0 ) cout << "Processed " << processed << " event ( found " << matching << " matching)\r" << flush;
//
//         lua_pushvalue ( lua, -1 );
//
//         lua_call ( lua, 0, 1 );
//         bool passedFilter = lua_toboolean ( lua, -1 );
//         lua_pop ( lua, 1 );
//
//         if ( passedFilter )
//         {
//             int np = ReadPayload<int> ( 0, 16, 26, 0, false );
//
//             int sample = 0;
//
//             for ( int i = 13; i < np; i++ )
//             {
//                 unsigned int amp = ReadPayload<unsigned int> ( i, 0, 13, 0, false );
//                 res->Fill ( sample, amp );
//                 sample++;
//
//                 amp = ReadPayload<unsigned int> ( i, 16, 29, 0, false );
//                 res->Fill ( sample, amp );
//                 sample++;
//             }
//
//             matching++;
//         }
//
//         processed++;
//
//         GetAWord ( "GEB", invertWords );
//
//         if ( binaryFileToRead.eof() && fnum < binFiles.size()-1 )
//         {
//             fnum++;
//             InitializePlotGEBWaveForm ( binFiles[fnum], "GEB", true );
//         }
//     }
//
//     cout << endl;
//     cout << "Done processing " << processed << " event ( found " << matching << " matching)\r" << endl;
//
//     res->Draw ( "colz" );
//
//     lua_settop ( lua, 0 );
//
//     return res;
// }

int compDiffCounter = 0;

ifstream binCompFile1;
ifstream binCompFile2;

void CompareGEBBinaries(bool strictComp, bool invertWords)
{
	std::pair<BinDecHeadStruct*, char*>* readFile1a = 0;
	std::pair<BinDecHeadStruct*, char*>* readFile1b = 0;
	std::pair<BinDecHeadStruct*, char*>* readFile2a = 0;
	std::pair<BinDecHeadStruct*, char*>* readFile2b = 0;

	if (!binCompFile1.is_open() || !binCompFile2.is_open())
	{
		return;
	}

	if (readFile1a == NULL)
	{
		readFile1a = new std::pair<BinDecHeadStruct*, char*>;
		readFile1a->first = new BinDecHeadStruct;
		readFile1a->second = new char[50000];

		readFile1b = new std::pair<BinDecHeadStruct*, char*>;
		readFile1b->first = new BinDecHeadStruct;
		readFile1b->second = new char[50000];

		readFile2a = new std::pair<BinDecHeadStruct*, char*>;
		readFile2a->first = new BinDecHeadStruct;
		readFile2a->second = new char[50000];

		readFile2b = new std::pair<BinDecHeadStruct*, char*>;
		readFile2b->first = new BinDecHeadStruct;
		readFile2b->second = new char[50000];
	}

	while (!binCompFile1.eof() && !binCompFile2.eof())
	{
		GetAWord(binCompFile1, invertWords);
		*readFile1a = *gebWord;

		GetAWord(binCompFile2, invertWords);
		*readFile2a = *gebWord;

		if (readFile1a->first->type != readFile2a->first->type || readFile1a->first->length != readFile2a->first->length
				|| readFile1a->first->timestamp != readFile2a->first->timestamp)
		{
			if (strictComp)
			{
				cout << "Difference found in the headers at word #" << compDiffCounter << "\n";
				cout << "Types are=> File 1: " << readFile1a->first->type << " / File 2: " << readFile2a->first->type << "\n";
				cout << "Lengths are=> File 1: " << readFile1a->first->length << " / File 2: " << readFile2a->first->length << "\n";
				cout << "Timestamps are=> File 1: " << readFile1a->first->timestamp << " / File 2: " << readFile2a->first->timestamp << "\n";

				compDiffCounter++;

				return;
			}
			else
			{
				GetAWord(binCompFile1, invertWords);
				*readFile1b = *gebWord;

				GetAWord(binCompFile2, invertWords);
				*readFile2b = *gebWord;

				if (readFile1a->first->type != readFile2b->first->type || readFile1a->first->length != readFile2b->first->length
						|| readFile1a->first->timestamp != readFile2b->first->timestamp || readFile1b->first->type != readFile2a->first->type
						|| readFile1b->first->length != readFile2a->first->length || readFile1b->first->timestamp != readFile2a->first->timestamp)
				{
					cout << "Difference found in the headers at word #" << compDiffCounter << "\n";
					cout << "Types are=> File 1: " << readFile1a->first->type << " / File 2: " << readFile2a->first->type << "\n";
					cout << "Lengths are=> File 1: " << readFile1a->first->length << " / File 2: " << readFile2a->first->length << "\n";
					cout << "Timestamps are=> File 1: " << readFile1a->first->timestamp << " / File 2: " << readFile2a->first->timestamp << "\n";

					compDiffCounter++;

					if (readFile1b->first->type != readFile2b->first->type || readFile1b->first->length != readFile2b->first->length
							|| readFile1b->first->timestamp != readFile2b->first->timestamp)
					{
						cout << "Difference found in the headers at word #" << compDiffCounter << "\n";
						cout << "Types are=> File 1: " << readFile1b->first->type << " / File 2: " << readFile2b->first->type << "\n";
						cout << "Lengths are=> File 1: " << readFile1b->first->length << " / File 2: " << readFile2b->first->length << "\n";
						cout << "Timestamps are=> File 1: " << readFile1b->first->timestamp << " / File 2: " << readFile2b->first->timestamp << "\n";

						compDiffCounter++;
					}

					return;
				}
			}
		}

		compDiffCounter++;
	}
}

void CompareGEBBinaries(string fName1, string fName2, bool strictComp, bool invertWords)
{
	compDiffCounter = 0;

	binCompFile1.open(fName1.c_str(), std::ios::in | std::ios::binary);
	binCompFile2.open(fName2.c_str(), std::ios::in | std::ios::binary);

	CompareGEBBinaries(strictComp, invertWords);

	return;
}
