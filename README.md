> [!NOTE]
> Builds for Linux, Darwin and Windows will be released in **releases** section soon.

Vstility is a simple command line interface that uses JUCE to apply VST3, VST and LADSPA plugins onto audio files. 

Out of the box, it:

1. Supports VST3 Plugins.
2. Supports VST (1-2) Plugins (WIP).
3. Supports LADSPA Plugins (WIP).

## Usage

```
Usage: vstility --input=<input_file> --output=<output_file> --vst3=<vst3_path>

Options:
  --input     <file>   Input audio file path
  --output    <file>   Output audio file path
  --vst3      <file>   VST3 plugin file path
  --y                  Overwrites the output file if exists without notice
  --help               Displays help message
```

> [!IMPORTANT]
> JUCE will throw an error if the plugin depends on another plugin which is not found. Thus, program will crash.

## Setting up

* [ ] `git clone` this repository.

* [ ] [Download CMAKE](https://cmake.org/download/) if you aren't already using it (Clion and VS2022 both have it bundled, so you can skip this step in those cases).

* [ ] Download JUCE if you haven't installed it yet. It will also download Projucer, you can use it to add modules or manage your project.

* [ ] Open `vstility.jucer` via Projucer and change JUCE installation path accordingly for your JUCE installation. Do not forget to save your project!

* [ ] Now, you can make changes to the `Main.cpp` file and build it via CMAKE. 
