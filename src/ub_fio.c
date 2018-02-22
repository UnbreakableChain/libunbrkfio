#include <ctype.h>
#include <stdio.h>
#include "ub_fio.h" 



enum UbIOErr skip_spaces_breaks_tabs(FILE* file);


enum UbIOErr skip_spaces_tabs(FILE* file);


enum UbIOErr end_line(FILE* file);


enum UbIOErr fread_chain_name(char** buffer, FILE* file);


enum UbIOErr fread_next_date(UbDate* date, FILE* file);


enum UbIOErr fread_next_link(UbLink* link, FILE* file);


enum UbIOErr fwrite_link(UbDate date, UbLink link, FILE* file);


enum UbIOErr fread_UbChain(UbChain** chain, FILE* file){
	enum UbIOErr err;

	char* buffer = 0; 
	err = fread_chain_name(&buffer, file);
	if (err != UB_IO_OK) return err;
	*chain = new_UbChain(buffer);
	free(buffer);

	skip_spaces_breaks_tabs(file);
	if (feof(file)) return UB_IO_OK;

	UbDate date;
	UbLink link;
	while (!feof(file)){
		err = fread_next_date(&date, file);
		if (err != UB_IO_OK) return err;
		err = fread_next_link(&link, file);
		if (err != UB_IO_OK) return err;
		add_UbLink_unsorted(*chain, link, date);
		skip_spaces_breaks_tabs(file);
	}
	sort_UbChain(*chain);
	
	return UB_IO_OK;
}


enum UbIOErr fread_chain_name(char** buffer, FILE* file){
	skip_spaces_breaks_tabs(file);
	int size = 20;
	*buffer = (char*)malloc(sizeof(char*) * size);
	int n_chars = getline(buffer, &size, file);
	if (n_chars <= 0){
		free(*buffer);
		printf("\nError name\n");
		return UB_IO_RERR;
	}
	if ((*buffer)[n_chars-1] == '\n') (*buffer)[n_chars-1] = '\0';

	return UB_IO_OK;
}


enum UbIOErr fread_next_date(UbDate* date, FILE* file){
	skip_spaces_tabs(file);

	int year, mon, day;
	int n = fscanf(file, "%d-%d-%d", &year, &mon, &day);
	if (n == EOF){
		printf("\nError date\n");
		return UB_IO_RERR;
	} else if (n < 3){
		printf("\nError date 2\n");
		return UB_IO_PERR;
	}
	
	*date = (UbDate){.year=year, .mon=mon, .day=day};
	return UB_IO_OK;
}


enum UbIOErr fread_next_link(UbLink* link, FILE* file){
	skip_spaces_tabs(file);

	int hardness;
	int n = fscanf(file, "%d", &hardness);
	if (n < 1){
		printf("\nError link 1\n");
		return UB_IO_PERR;
	} else if (n == EOF) {
		printf("\nError link 2\n");
		return UB_IO_RERR;
	}

	link->flags = 0;
	skip_spaces_tabs(file);
	char c = 0;
	n = fscanf(file, "ghos%c%*[^\n]", &c); 
	if (c == 't'){
		link->flags = UB_GHOST_LINK;
	} else if (n == EOF && !feof(file)) {
		return UB_IO_RERR;
	}
	link->hardness = hardness;

	return UB_IO_OK;
}


enum UbIOErr skip_spaces_breaks_tabs(FILE* file){
	int c = fgetc(file);

	while (isspace(c) || c == '\n' || c == '\r' || c == '\t'){
		c = fgetc(file);
		if (c == EOF && !feof(file)){
			return UB_IO_RERR;
		}
	}

	if (ungetc(c, file) == EOF){
		return UB_IO_RERR;
	}

	return UB_IO_OK; 
}


enum UbIOErr skip_spaces_tabs(FILE* file){
	int c = fgetc(file);

	while (isspace(c) || c == '\t'){
		c = fgetc(file);
		if (c == EOF && !feof(file)){
			return UB_IO_RERR;
		}
	}

	if (ungetc(c, file) == EOF){
		return UB_IO_RERR;
	}

	return UB_IO_OK; 
}


enum UbIOErr end_line(FILE* file){
	if (fscanf(file, "%*[^\n]") == EOF && !feof(file)) return UB_IO_RERR;
	return UB_IO_OK;
}


enum UbIOErr fwrite_UbChain(UbChain* chain, FILE* file){
	enum UbIOErr err;

	if (fprintf(file, "%s\n", chain->name) < 0){
		return UB_IO_WERR;
	}

	UbChainIterator* iter;
	UbChain_iter_first(chain, &iter);
	while (iter != NULL){
		err = fwrite_link(int_to_UbDate(iter->date), iter->link, file);
		if (err != UB_IO_OK) return err;
		UbChain_iter_next(&iter);
	}
	free(iter);

	return UB_IO_OK;
}


enum UbIOErr fwrite_link(UbDate date, UbLink link, FILE* file){
	if (fprintf(file, "\n%d-%d-%d %d", date.year, date.mon, date.day, 
            link.hardness) < 0){
		return UB_IO_WERR;
	}
	if (link.flags & (UB_GHOST_LINK != 0)){
		if(fputs(" ghost", file) == EOF){
			return UB_IO_WERR;
		}
	}

	return UB_IO_OK;
}
