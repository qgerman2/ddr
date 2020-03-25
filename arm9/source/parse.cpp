#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <bitset>
#include "parse.h"
#include "play.h"
#include <cmath>

using namespace std;

static const struct t_pair empty_tag;
static const struct t_bpm empty_bpm;
static const measure empty_measure;

const string partialTags[6] = {
	"TITLE",
	"ARTIST",
	"BANNER",
	"BACKGROUND",
	"MUSIC",
	"NOTES",
};

songdata::~songdata() {
	cout << "\neliminado songdata";
	int e = 0;
	for (auto m = this->notes.begin(); m != this->notes.end(); m++) {
		for (auto n = m->begin(); n != m->end(); n++) {
			e++;
			delete [] *n;
		}
	}
	cout << "\neliminao " << e;
}

metadata parseSimFile(string path, bool partial) {
	metadata tags;
	enum class state {IDLE, KEY, VALUE};
	FILE *fp = fopen(path.c_str(), "r");
	int p;
	bool prevslash = FALSE;
	bool skip = FALSE;
	bool partialskip = false;
	int notescount = 0;
	state task = state::IDLE;
	string buffer = "";
	struct t_pair tag = empty_tag;
	do {
		p = fgetc(fp);
		if (feof(fp))
			break;
		if (!skip) {
			if ((task == state::KEY || task == state::VALUE) && (p != ';')) {
				if (p == '/') {
					if (prevslash) {
						skip = TRUE;
						prevslash = FALSE;
					} else {
						prevslash = TRUE;
					}
				}
				else {
					if (prevslash) {
						prevslash = FALSE;
						if (!partialskip) {
							buffer.append(1, '/');
						}
					}
					if (((task == state::VALUE) && (p == ':')) || (p != ':'))
						if (!partialskip) {
							buffer.append(1, p);
						}
				}
			}
			switch (p) {
				case '#':
					if (task == state::IDLE)
						task = state::KEY;
					break;
				case ':':
					if (task == state::KEY) {
						task = state::VALUE;
						tag.key = buffer;
						buffer = "";
						if (partial) {
							partialskip = true;
							for (int i = 0; i < 6; i++) {
								if (partialTags[i] == tag.key) {
									partialskip = false;
									break;
								}
							}
						}
					}
					else if (task == state::VALUE) {
						if (partial && (tag.key == "NOTES")) {
							notescount++;
							if (notescount > 3) {
								partialskip = true;
							} 
						}
					}
					break;
				case ';':
					if (task == state::VALUE) {
						task = state::IDLE;
						tag.value = buffer;
						if (!partialskip || (tag.key == "NOTES")) {
							tags.push_back(tag);
						}
						partialskip = false;
						notescount = 0;
						tag = empty_tag;
						buffer = "";
					}
					break;
			}
		} else {
			if (p == '\n') {
				buffer.append(1, '\n');
				skip = FALSE;
			}
		}
	} while (1);
	fclose(fp);
	return tags;
}

songdata parseSong(metadata* tags) {
	songdata song;
	song.tags = tags;
	for (auto i = tags->begin(); i != tags->end(); i++) {
		if (i->key == "NOTES") {
			song.notes = parseNotes(&i->value);
			continue;
		}
		if (i->key == "BPMS") {
			song.bpms = parseBPMS(&i->value, FALSE);
			continue;
		}
		if (i->key == "STOPS") {
			song.stops = parseBPMS(&i->value, TRUE);
			continue;
		}
	}
	return song;
}

bpmdata parseBPMS(string* data, bool isStops) {
	bpmdata bpms;
	struct t_bpm bpm = empty_bpm;
	string buffer;
	enum class state {IDLE, KEY, VALUE};
	state task = state::IDLE;
	char c;
	for (uint i = 0; i < data->size(); i++) {
		c = (*data)[i];
		switch (task) {
			case state::KEY:
				if (c != '=') {
					buffer.append(1, c);
				} else {
					task = state::VALUE;
					bpm.beatf = stod(buffer) * beatfperiod;
					buffer = "";
				}
				break;
			case state::VALUE:
				if (c != ',' && c != '\n') {
					buffer.append(1, c);
				} 
				if ((c == ',') || (c == '\n') || i == data->length() - 1) {
					if (i == data->length() - 1) {
						buffer.append(1, c);
					}
					task = state::IDLE;
					if (isStops)
						bpm.bpmf = (stod(buffer) * (1 << MINUTEFRAC)) / 60;
					else
						bpm.bpmf = stod(buffer) * pow(2, BPMFRAC);
					buffer = "";
					bpms.push_back(bpm);
					bpm = empty_bpm;
				}
				break;
			case state::IDLE:
				if (c != ' ' && c != '\n' && c != ',') {
					task = state::KEY;
					buffer.append(1, c);
				}
				break;
		}
	}
	return bpms;
}


notedata parseNotes(string* data) {
	size_t s = data->find(':');
	size_t e = data->find(';');
	for (int v = 0; v <= 3; v++) {
		s = data->find(':', s + 1);
	}
	string rawnotes = data->substr(s, e - s);
	char c;
	notedata notes;
	measure m = empty_measure;
	int fourcount = 0;
	int count = -1;
	int k = 0;
	for (uint i = 0; i < rawnotes.size(); i++){
		c = rawnotes[i];
		if ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == 'M')) {
			count++;
			if (count > 3) {
				count = 0;
				fourcount += 1;
				if (fourcount > 3) {
					fourcount = 0;
				}	
			}
			if ((count == 0) && (fourcount == 0)) {
				m.push_back(new u16[4]);
				k++;
				for (int i = 0; i <= 3; i++) {
					m.back()[i] = 0;
				}	
			}
		}
		//printf("%c", c);
		switch (c) {
			case '1':
				m.back()[fourcount] |= normal[count];
				break;
			case '2':
				m.back()[fourcount] |= holdhead[count];
				break;
			case '3':
				m.back()[fourcount] |= holdtail[count];
				break;
			case 'M':
				m.back()[fourcount] |= mine[count];
				break;
			case ',':
				notes.push_back(m);
				m = empty_measure;
				break;
		}
	}
	cout << "\ncreado " << k;
	notes.push_back(m);
	size_t size = 0;
	for ( auto i = notes.begin(); i != notes.end(); i++ ) {
		size = size + i->size();
	}
	return notes;
} 