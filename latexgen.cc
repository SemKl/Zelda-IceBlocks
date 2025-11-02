// This program can be used to convert a txt representation of an
// ice block puzzle to a tikzpicture to be used in LateX.

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

int main()
{
	string infile;
	string outfile;
	cout << "Input puzzle file to convert to LaTeX Tikzpicture: ";
	cin >> infile;
	cout << "Output file to save LaTeX Tikzpicture: ";
	cin >> outfile;
	string line;
	ifstream input(infile);
	ofstream output(outfile);
	size_t x, y;
	if (input.is_open())
	{
		output << "\\begin{tikzpicture}[every node/.style={minimum size=1cm-\\pgflinewidth, outer sep=0pt}]\n";
		getline(input, line, ' ');
		x = atoi(line.c_str());
		getline(input, line);
		y = atoi(line.c_str());
		output << "\\draw[step=1cm,color=black] (0,0) grid (" << x << "," << y << ");\n";

		size_t j = 0;
		while (getline(input, line) && j < y)
		{
			for (size_t i = 0; i < min(x, line.length()); i++)
			{
				if (line[i] == 'O' || line[i] == 'o')
				{
					output << "\t\\node[fill=iceblock] at (" << i << ".5"
						   << "," << y - 1 - j << ".5) {};\n";
				}
				if (line[i] == 'T' || line[i] == 't')
				{
					output << "\t\\draw[target,fill=target] (" << i << ".5"
						   << "," << y - 1 - j << ".5) circle (0.75ex);\n";
					output << "\t\\draw[-,target] (" << i << "," << y - 1 - j << ") -- (" << i + 1 << "," << y - 1 - j << ") -- (" << i + 1 << "," << y - 1 - j + 1 << ") -- (" << i << "," << y - 1 - j + 1 << ") -- (" << i << "," << y - 1 - j << ");\n";
					output << "\t\\draw[-,target] (" << i << "," << y - 1 - j << ") -- (" << i + 1 << "," << y - 1 - j + 1 << ");\n";
					output << "\t\\draw[-,target] (" << i << "," << y - 1 - j + 1 << ") -- (" << i + 1 << "," << y - 1 - j << ");\n";
				}
				if (line[i] == 'H' || line[i] == 'h')
				{
					output << "\t\\node[fill=iceblock] at (" << i << ".5"
						   << "," << y - 1 - j << ".5) {};\n";
					output << "\t\\draw[target,fill=target] (" << i << ".5"
						   << "," << y - 1 - j << ".5) circle (0.75ex);\n";
					output << "\t\\draw[-,target] (" << i << "," << y - 1 - j << ") -- (" << i + 1 << "," << y - 1 - j << ") -- (" << i + 1 << "," << y - 1 - j + 1 << ") -- (" << i << "," << y - 1 - j + 1 << ") -- (" << i << "," << y - 1 - j << ");\n";
					output << "\t\\draw[-,target] (" << i << "," << y - 1 - j << ") -- (" << i + 1 << "," << y - 1 - j + 1 << ");\n";
					output << "\t\\draw[-,target] (" << i << "," << y - 1 - j + 1 << ") -- (" << i + 1 << "," << y - 1 - j << ");\n";
				}
				if (line[i] == 'X' || line[i] == 'x')
				{
					output << "\t\\node[fill=floor] at (" << i << ".5"
						   << "," << y - 1 - j << ".5) {};\n";
				}
				if (line[i] == 'W' || line[i] == 'w')
				{
					output << "\t\\node[fill=wall] at (" << i << ".5"
						   << "," << y - 1 - j << ".5) {};\n";
				}
			}
			j++;
		}
		output << "\\end{tikzpicture}";
	}
	return 0;
}
