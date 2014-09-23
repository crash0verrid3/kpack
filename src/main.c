#include "common.h"
#include "packager.h"

void displayUsage() {
	printf(
		"KnightOS package manager - packs or unpacks a KnightOS package from or into a directory model.\n"
		"\n"
		"Usage: kpack [-c|--config configFile] [-e|--extract] [-s|--sum crc16|sha1|md5|none] [-x|--compressor pucrunch|rle|none] _package_ _model_\n"
		"See `man 1 kpack` for details.\n"
		"\n"
		"Options:\n"
		"\t-c|--config\n"
		"\t\tSpecifies an alternate config file.\n"
		"\t-e|--extract\n"
		"\t\tExtracts a package instead of creating one.\n"
		"\t-s|--sum\n"
		"\t\tSpecifies a checksum algorithm to use, among 'crc16', 'sha1', 'md5' and 'none', defaulting to 'crc16'\n"
		"\t-x|--compressor\n"
		"\t\tSpecifies the compressor to use among 'pucrunch', 'rle' and 'none', defaulting to 'pucrunch'\n"
	);
}

int main(int argc, char **argv) {
	// File parsing
	DIR *rootDir;
	
	initRuntime();
	
	if (parse_args(argc, argv)) {
		printf("Aborting operation.\n");
		return 1;
	}
	
	printf("\nArgument parsing successful.\n\n");
	
	// Pack things
	if (packager.pack) {
		if (parse_metadata()) {
			printf("Aborting operation.\n");
			if (packager.pkgname) {
				free(packager.pkgname);
			}
			if (packager.repo) {
				free(packager.repo);
			}
			fclose(packager.config);
			return 1;
		}
		
		printf("Metadata parsing successful.\nPackaging into file '%s'\n\n", packager.filename);
		
		packager.output = fopen(packager.filename, "wb");
		
		// See doc/package_format for information on package format
		fputs("KPKG", packager.output);
		fputc(KPKG_FORMAT_VERSION, packager.output);
		
		// Write metadata
		fputc(packager.mdlen, packager.output);
		// Package name
		fputc(KEY_PKG_NAME, packager.output);
		fputc(strlen(packager.pkgname), packager.output);
		fputs(packager.pkgname, packager.output);
		// Package repo
		fputc(KEY_PKG_REPO, packager.output);
		fputc(strlen(packager.repo), packager.output);
		fputs(packager.repo, packager.output);
		// Package version
		fputc(KEY_PKG_VERSION, packager.output);
		fputc(3, packager.output);
		fputc(packager.version.major, packager.output);
		fputc(packager.version.minor, packager.output);
		fputc(packager.version.patch, packager.output);
		
		// Write files
		rootDir = opendir(packager.rootName);
		if(!rootDir) {
			printf("Couldn't open directory %s.\nAborting operation.\n", packager.rootName);
		} else {
			writeModel(rootDir, packager.rootName);
			closedir(rootDir);
		}
		
		free(packager.pkgname);
		free(packager.repo);
		
		fclose(packager.output);
		
		fclose(packager.config);
		
		printf("Packing done !\n");
	} else {
		printf("Unpacking not supported yet.\n");
	}
	
	return 0;
}
