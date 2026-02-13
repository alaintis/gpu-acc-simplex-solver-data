#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#include "mps.h"
#include "mps_io.h"
#include "mps_normalize.h"
#include "mps_csc.h"

int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

void process_file(const char *input_path, const char *output_path, int csc) {
    fprintf(stderr, "Processing: %s\n", input_path);
    FILE *f = fopen(input_path, "r");
    if (!f) {
        perror("Could not open input file");
        return;
    }

    FILE *out = stdout;
    if (output_path) {
        out = fopen(output_path, "w");
        if (!out) {
            perror("Could not open output file");
            fclose(f);
            return;
        }
    }

    mps_model_t model;
    init_model(&model);
    parse_mps(f, &model);
    fclose(f);

    normalize(&model);


    if(csc) {
        write_csc(out, &model);
    } else {
        write_mps(out, &model);
    }
    if (out != stdout) fclose(out);
}

void process_directory(const char *input_dir, const char *output_dir, int csc) {
    DIR *d = opendir(input_dir);
    if (!d) {
        perror("Could not open directory");
        return;
    }

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        // skip . and ..
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        // filter for .mps files
        const char *ext = strrchr(ent->d_name, '.');
        if (!ext || strcmp(ext, ".mps") != 0)
            continue;

        char input_path[4096];
        snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, ent->d_name);

        char output_path[4096];
        snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, ent->d_name);

        if(csc) {
            int len = strlen(output_path);
            strcpy(&output_path[len -4], ".csc");
        }

        process_file(input_path, output_path, csc);
    }

    closedir(d);
}

int print_usage(char *bin) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [--csc] file.mps [normalized.mps]\n", bin);
    fprintf(stderr, "  %s [--csc] directory/ output_directory/\n", bin);
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return print_usage(argv[0]);
    }

    int csc = 0;
    int arg_index = 1;

    if (strcmp(argv[arg_index], "--csc") == 0) {
        csc = 1;
        arg_index++;
    }

    if (argc - arg_index < 1 || argc - arg_index > 2) {
        return print_usage(argv[0]);
    }

    const char *input = argv[arg_index];
    const char *output = (argc-arg_index >= 2) ? argv[arg_index + 1] : NULL;

    if (is_directory(input)) {
        // directory mode
        if (!output || !is_directory(output)) {
            fprintf(stderr, "Output path must be an existing directory when input is a directory.\n");
            return 1;
        }
        process_directory(input, output, csc);
    } else {
        // single-file mode
        process_file(input, output, csc);
    }

    return 0;
}

