#include <JuceHeader.h>

using namespace juce;

class AudioFileProcessor : public JUCEApplication {
public:
    const String getApplicationName() override { return "vstility"; }
    const String getApplicationVersion() override { return "1.0.0"; }

    void initialise(const String& commandLine) override
    {

        StringArray argsArray = StringArray::fromTokens(commandLine, true);

        String executable = argsArray[0];

        ArgumentList args(executable, argsArray);

        if (args.containsOption("--help"))
        {
            printHelp();
            quit();
            return;
        }


        if (!args.containsOption("--input") || !args.containsOption("--output") || !args.containsOption("--vst3")) {
            std::cout << "Error: Missing required arguments." << std::endl;
            printHelp();
            quit();
            return;
        }

        File inputFile = args.getFileForOption("--input");
        File outputFile = args.getFileForOption("--output");
        File vst3File = args.getFileForOption("--vst3");

        if (inputFile == File() || outputFile == File() || vst3File == File())
        {
            std::cout << "Error: Missing required arguments." << std::endl;
            printHelp();
            quit();
            return;
        }


        if (outputFile.existsAsFile() && !args.containsOption("--y")) {
            if (!confirmOverwrite())
            {

                std::cout << "Operation cancelled." << std::endl;
                quit();
                return;
            }
        }

        if (!inputFile.existsAsFile() || !vst3File.existsAsFile())
        {
            std::cout << "Error: Input file or VST3 file not found." << std::endl;
            quit();
            return;
        }

        processAudioFile(inputFile, outputFile, vst3File);
        quit();
    }

    void shutdown() override {}
private:

    void printHelp()
    {
        std::cout << "Usage: vstility --input=<input_file> --output=<output_file> --vst3=<vst3_path>" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --input     <file>   Input audio file path" << std::endl;
        std::cout << "  --output    <file>   Output audio file path" << std::endl;
        std::cout << "  --vst3      <file>   VST3 plugin file path" << std::endl;
        std::cout << "  --y                  Overwrites the output file if exists without notice" << std::endl;
        std::cout << "  --help               Display this help message" << std::endl;
    }

    bool confirmOverwrite()
    {
        while (true)
        {
            std::cout << "Output file already exists. Overwrite? [y/n]: ";
            std::string input;
            std::getline(std::cin, input);

            if (input.length() == 1)
            {
                char choice = std::tolower(input[0]);
                if (choice == 'y')
                    return true;
                else if (choice == 'n')
                    return false;
            }

            std::cout << "Invalid input. Please enter 'y' or 'n'." << std::endl;
        }
    }

    void processAudioFile(const File& inputFile, const File& outputFile, const File& vst3File) {
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(inputFile));

        if (reader == nullptr)
        {
            std::cout << "Could not read input file." << std::endl;
            return;
        }

        AudioPluginFormatManager pluginFormatManager;
        pluginFormatManager.addDefaultFormats();

        KnownPluginList pluginList;
        OwnedArray<PluginDescription> foundPlugins;
        VST3PluginFormat vst3Format;

        vst3Format.findAllTypesForFile(foundPlugins, vst3File.getFullPathName());

        if (foundPlugins.isEmpty())
        {
            std::cout << "Could not find VST3 plugin." << std::endl;
            return;
        }

        String errorMessage;

        std::unique_ptr<AudioPluginInstance> plugin(pluginFormatManager.createPluginInstance(*foundPlugins[0],
            reader->sampleRate, 512, errorMessage));

        if (plugin == nullptr)
        {
            std::cout << "Could not create plugin instance: " << errorMessage << std::endl;
            return;
        }

        plugin->prepareToPlay(reader->sampleRate, reader->numChannels);

        AudioBuffer<float> buffer(reader->numChannels, reader->lengthInSamples);

        reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

        MidiBuffer midiBuffer;

        plugin->processBlock(buffer, midiBuffer);

        WavAudioFormat wavFormat;
        std::unique_ptr<FileOutputStream> fileStream(outputFile.createOutputStream());

        if (fileStream == nullptr)
        {
            std::cout << "Error: Could not create output file stream." << std::endl;
            return;
        }
        else {
            // Overwrite the file
            fileStream->setPosition(0);
            fileStream->truncate();
        }

        std::unique_ptr<AudioFormatWriter> writer(wavFormat.createWriterFor(fileStream.release(),
            reader->sampleRate, reader->numChannels, 16, {}, 0));

        if (writer == nullptr)
        {
            std::cout << "Error: Could not create output file writer." << std::endl;
            return;
        }

        writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());

        std::cout << "Processing complete. Output saved to: " << outputFile.getFullPathName() << std::endl;
    }
};

START_JUCE_APPLICATION(AudioFileProcessor);