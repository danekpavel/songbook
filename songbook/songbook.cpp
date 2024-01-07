#include "SongbookConverter.hpp"
#include "SongbookPrinterLatex.hpp"
#include "SongbookException.hpp"

#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <clocale>

/**
 * Used for storing parameters from command line arguments.
 */
struct StartupArgs {
    std::string xml_file;      /**< input XML file */
    std::string latex_file;    /**< output LaTeX file*/
    int pdf{0};                /**< number of times XeLaTeX should be run */
};

/**
 * Prints program usage to `std::cerr`.
 */
void print_usage() {
    std::cerr << R"(Usage:  songbook [options] <input_xml_file>
Options:
  -l <file>     Save LaTeX source code to <file>. Standard output is used when 
                output file is not specified and '-pdf[2]' is not used.
  -pdf          Run xelatex to produce a PDF. XeTeX must be installed and 
                available to the program. PDF file name is based on the LaTeX 
                file name. If '-l' was not used, LaTeX file name is derived
                from the XML file name by removing the '.xml' extension (when 
                present) and adding the '.tex' extension.
  -pdf2         Run XeLaTeX twice to properly generate the table of contents. 
                See '-pdf' for other details. Only one of '-pdf'/'-pdf2' can be 
                used.
)";
}

/**
 * Processes command line arguments.
 * 
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @return processed arguments
 */
StartupArgs process_args(int argc, char* argv[]) {

    using namespace std::literals;
    StartupArgs args;

    int i{1};
    while (i < argc) {
        if (argv[i] == "-l"s) {
            if (i+1 == argc) 
                throw std::runtime_error("LaTeX file name missing after '-l'");
            if (!args.latex_file.empty())
                throw std::runtime_error("LaTeX file ('-l') specified more than once");
            args.latex_file = argv[i+1];
            i += 2;
        } else if (argv[i] == "-pdf"s || argv[i] == "-pdf2"s) {
            if (args.pdf > 0) 
                throw std::runtime_error("more than one usage of '-pdf' or '-pdf2'");
            args.pdf = (argv[i] == "-pdf"s) ? 1 : 2;
            ++i;
        } else if (i == argc-1) {  // last argument left -> input file name
            args.xml_file = argv[i];
            ++i;
        } else {
            throw std::runtime_error("incorrect argument(s)");
        }
    }

    // arguments parsed correctly but input file not specified
    if (args.xml_file.empty()) 
        throw std::runtime_error("input XML file not specified");

    // generate LaTeX file name when not given but LaTeX file is produced
    if (args.pdf && args.latex_file.empty()) {
        std::string latex_file{args.xml_file};
        // remove .xml extension when present (first make lowercase)
        if (latex_file.size() > 4) {
            std::string extension = latex_file.substr(latex_file.size() - 4);
            std::transform(begin(extension), end(extension), begin(extension),
                [](unsigned char c){ return std::tolower(c); });
            if (extension == ".xml")
                latex_file.erase(latex_file.size() - 4);
        }
        latex_file.append(".tex");
        args.latex_file = latex_file;
    }

    return args;
}

int main(int argc, char* argv[]) {

    using namespace songbook;

    // with no command line arguments print usage and exit 
    if (argc == 1) {
        print_usage();
        return 0;
    }

    // parse command line arguments
    StartupArgs args;
    try {
        args = process_args(argc, argv);
    } catch (std::runtime_error& e) {
        std::cerr << "Error during parsing command line arguments: " <<
            e.what() << "\n\n";
        print_usage();
        return 1;
    }

    try {
        SongbookConverter converter = init_converter<SongbookPrinterLatex>();
        converter.parse_songbook(args.xml_file);

        // send output to a file when name was or to std::cout otherwise
        std::ofstream ofs;
        if (!args.latex_file.empty()) {
            ofs.open(args.latex_file);
            if (!ofs.is_open())
                throw std::runtime_error("Output file " + args.latex_file + " cannot be opened");
        }
        std::ostream& output = (ofs.is_open() ? ofs : std::cout);

        output << converter.convert();

        // run XeLaTeX once or twice
        if (args.pdf) {
            ofs.close();
            std::string command{"xelatex " + args.latex_file};
            for (int i = 0; i < args.pdf && i < 2; ++i) {
                std::cerr << "Running XeLaTeX: " << command << "\n";
                std::system(command.c_str());
            }
        }
    } 
    catch (SongbookException& ce) {
        std::cerr << ce.what();
        return 1;
    } 
    catch (std::exception& e) {
        std::cerr << e.what();
        return 1;
    } 
    catch (...) {
        std::cerr << "Unknown error";
        return 1;
    }
}
