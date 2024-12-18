#include <iostream>
#include <csignal>
#include <cstdio>

#include "fluid.h"
#include "flags-parser.h"

bool save_flag = false;
bool exit_flag = false;

void my_handler(int nsig) {
    save_flag = true;
}


int main(int argc, char* argv[]) {
    (void) signal(SIGINT, my_handler);

    parser optsParser(argc, argv);

    auto input_file = optsParser.get_option("--input-file");
    auto save_file = optsParser.get_option("--save-file");
    int p_type = get_type(optsParser.get_option("--p-type"));
    int v_type = get_type(optsParser.get_option("--v-type"));
    int v_flow_type = get_type(optsParser.get_option("--v-flow-type"));

    std::ifstream input(input_file);
    if (!input.is_open()) {
        throw std::invalid_argument("Can't open file");
    }
    int N, M;
    input >> N >> M;
    input.seekg(0, ios::beg);

    auto fluid = create_fluid(p_type, v_type, v_flow_type, N, M);

    std::ofstream saveFile(save_file);
    if (!input.is_open()) {
        throw std::invalid_argument("Can't open file");
    }


    fluid->load(input);
    int T = 1'000'000;
    for (int i = 0; i < T; ++i) {
        if (save_flag) {
            std::cout << "\nSaving current position..." << std::endl;
            saveFile.close();
            saveFile.open(save_file, std::ios::trunc);
            fluid->save(saveFile);
            save_flag = false;
            std::cout << "Saved to " + save_file << std::endl;


            char choice;
            do {
                std::cout << "Enter 'C' to continue or 'Q' to quit: ";
                std::cin >> choice;
                choice = std::tolower(choice);

                if (choice == 'q') {
                    exit_flag = true;
                    break;
                } else if (choice == 'c') {
                    break;
                } else {
                    std::cout << "Invalid input. Please enter 'C' or 'Q'." << std::endl;
                }
            } while (true);

            if (exit_flag) {
                break;
            }
        }

        fluid->next(std::cout);
    }


    input.close();
    saveFile.close();
    return 0;
}