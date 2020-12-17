/*
 Dial Lexing library
 Copyright (c) 2020 - present, Oluwadare Beloved
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
 */

#ifndef LEXER_LIB
#define LEXER_LIB

#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <functional>


namespace dial {
	using std::back_inserter;
	using std::copy;
	using std::string;
	using std::vector;

	//@DialLexerException Custom exception type for lexer
	struct DialLexerException : public std::runtime_error
	{
		string build_error_message(string message, size_t line, size_t position, bool is_validating) {
			string error_message = "";
			error_message.append(message);
			if (is_validating) {
				error_message.append(" occured while validating tokens ");
			}
			else {
				error_message.append(" at position ").append(std::to_string(position).append(" at line ").append(std::to_string(line)));
			}
			return error_message;
		}
		DialLexerException() : std::runtime_error(build_error_message("", -1, -1, false)) {
		}
		DialLexerException(std::string const& message, size_t line, size_t position, bool is_validating)
			: std::runtime_error("")
		{
			add_info(message, line, position, is_validating);
		}
		void add_info(std::string const& message, size_t line, size_t position, bool is_validating) {
			error_message.append(build_error_message(message, line, position, is_validating));
		}
		const char* what() const noexcept override {
			return error_message.c_str();
		}
	private:
		string error_message = "Lexer Error :\n";
	};

	//@TokenType -> Prototype constant to be overriden by user
	enum class TokenType : int;

	const string IS_NUMBER = "IS_NUMBER";
	const string IS_IDENTIFIER_ALPHA_NUM = "IS_IDENTIFIER_ALPHA_NUM";
	const string IS_IDENTIFIER_ALPHA_LOWER = "IS_IDENTIFIER_ALPHA_LOWER";
	const string IS_IDENTIFIER_ALPHA_UPPER = "IS_IDENTIFIER_ALPHA_UPPER";

	//@DIAL_LEXER_VALUE Semantic value type 
	//
	//@DIAL_NONE -> type NONE with no semantic value associate
	//@DIAL_NUMBER_DOUBLE -> type DOUBLE with number type value associate
	//@DIAL_IDENTIFIER -> type IDENTIFIER with identifier type value associate
	//@DIAL_STRING_START -> type STRING_START will resolve to @DIAL_STRING start
	//@DIAL_STRING_END -> type STRING_END will resolve to @DIAL_STRING end
	//@DIAL_STRING -> type STRING with string type value associate
	enum class DIAL_LEXER_VALUE : int {
		DIAL_NONE,
		DIAL_NUMBER_DOUBLE,
		DIAL_IDENTIFIER,
		DIAL_STRING_START,
		DIAL_STRING_END,
		DIAL_STRING
	};

	//@LexerType Type for lexer
	//
	//@REGEX -> Regex type
	//@RAW -> Hand crafted lexer type
	enum class LexerType {
		REGEX,
		RAW
	};

	//@Token Represent the token

	class Token {
	public:
		//@type  User defined type 
		//@lexeme  String content representing the value
		//@value  Semantic meaning for value
		Token(TokenType type, string lexeme, DIAL_LEXER_VALUE value)
		{
			this->type = type;
			this->lexeme = lexeme;
			this->value = value;
		}
	
		// Represent empty token
		Token()
		{
			this->line = -1;
			this->lexeme = "";
			this->value = static_cast<DIAL_LEXER_VALUE>(-1);
			this->type = static_cast<TokenType>(-1);
		}
		//@get_type returns token type
		TokenType get_type()
		{
			return this->type;
		}
		//@get_value returns token value
		DIAL_LEXER_VALUE get_value()
		{
			return this->value;
		}
		//@get_lexeme returns token lexeme
		string get_lexeme()
		{
			return this->lexeme;
		}
		//@get_line returns token line
		int get_line()
		{
			return this->line;
		}
		void set_line(int line)
		{
			this->line = line;
		}
		//@== token comparison
		bool operator==(const Token& other)
		{
			return other.lexeme == lexeme && other.type == type && other.line == line;
		}

	private:
		int line;
		TokenType type;
		string  lexeme;
		DIAL_LEXER_VALUE value;
	};

	DialLexerException lex_exception = DialLexerException();

	//@contains_ele utility function to check if a value exist in a container
	//
	//@list containter holding values
	//@value value item searched for
	template<typename C>
	bool contains_ele(const std::vector<C>& list, const C& value) {
		for (auto iter = list.begin(); iter < list.end(); iter++) {
			if (value != *iter)
				continue;
			else
				return true;
		}
		return false;
	}

	//@split_by_delimeter utility function to split a string object in respect to a char delimeter
	//
	//@txt elements to be splitted
	//@strs container to split @txt into
	//@ch delimeter used for splitting
	size_t split_by_delimeter(const std::string& txt, std::vector<std::string>& strs, char ch)
	{
		size_t pos = txt.find(ch), init_pos = 0;
		strs.clear();
		while (pos != std::string::npos) {
			strs.push_back(txt.substr(init_pos, pos - init_pos));
			init_pos = pos + 1;
			pos = txt.find(ch, init_pos);
		}
		string final_val = txt.substr(init_pos, std::min(pos, txt.size()) - init_pos + 1);
		if (!final_val.empty()) {
			strs.push_back(final_val);
		}
		return strs.size();
	}

	//@validate_double utility function to validate double precision value 
	//from a string value by extracting
	//
	//@val value to be extracted
	bool validate_double(string val) {
		double rez = 0, factor = 1;
		size_t counter = 0;
		if (val.at(counter) == '-') {
			counter++;
			factor = -1;
		}
		if (val.at(counter) == '.' || isdigit(val.at(counter))) {
			bool decimal_point = false;
			for (counter; counter < val.size(); counter++) {
				if (val.at(counter) == '.') {
					continue;
				}
				int digit = val.at(counter) - '0';
				if (digit >= 0 && digit <= 9) {
					if (decimal_point) {
						factor /= 10.0f;
					}
					rez = rez * 10.0f + float(digit);
				}
				else {
					return false;
				}
			}
			return true;
		}

		return false;
	};

	//@get_token utility function to get a Token object using lexer value @key_val as key
	//
	//@tokens a list of input tokens
	//@key_val a key used in retrieving a token
	[[nodiscard]] Token get_token(vector<Token>& tokens, const DIAL_LEXER_VALUE key_val) {
		auto type_ptr = std::find_if(tokens.begin(), tokens.end(), [&](Token token) { return token.get_value() == key_val; });
		return type_ptr != tokens.end() ? (*type_ptr) : Token();
	}

	//@get_type utility function to get a TokenType using lexer value @key_val as key
	//
	//@tokens a list of input tokens
	//@key_val a key used in retrieving a token
	[[nodiscard]] TokenType get_type(vector<Token>& tokens, const DIAL_LEXER_VALUE key_val) {
		auto token = get_token(tokens, key_val);
		return  token == Token() ? TokenType() : token.get_type();
	}

	//@verify_raw_tokens_integrity utility function to validate a list of input token for raw lexer type
	//
	//@input_tokens a list of input tokens
	//@except a string containing exception message
	//@identifiers a list of identifier token filtered for it's lexeme
	//@comment_start begining of comment token value
	//@comment_end end of comment token value
	bool verify_raw_tokens_integrity(vector<Token>& input_tokens, string& except, vector<string>& identifiers, string comment_start, string comment_end) {
		string initial_content = string(except);
		TokenType start_type = get_type(input_tokens, DIAL_LEXER_VALUE::DIAL_STRING_START);
		TokenType end_type = get_type(input_tokens, DIAL_LEXER_VALUE::DIAL_STRING_END);
		if ((start_type != TokenType() && end_type == TokenType()) || (start_type == TokenType() && end_type != TokenType())) {
			except.append("string tokens must have starting and ending tokens \n ");
		}
		TokenType string_type = get_type(input_tokens, DIAL_LEXER_VALUE::DIAL_STRING);
		if (string_type != TokenType()) {
			except.append("string token is not valid for use \n ");
		}

		if (!comment_start.empty() && comment_end == comment_start) {
			except.append("cant have same token as start and end comment");
		}
		for (auto token : input_tokens) {
			int counter = 0;
			for (auto token_2 : input_tokens) {
				if (token.get_value() == token_2.get_value()) {
					++counter;
				}
			}
			if (counter > 1 && !(token.get_value() == DIAL_LEXER_VALUE::DIAL_NONE)) {
				except.append("tokens must be unique except for NONE value -> " + token.get_lexeme() + "\n");
			}
		}
		Token identifier_token = get_token(input_tokens, DIAL_LEXER_VALUE::DIAL_IDENTIFIER);
		if (!(identifier_token == Token())) {
			identifiers.clear();
			string lex = identifier_token.get_lexeme();
			size_t n_split = split_by_delimeter(lex, identifiers, '|');
			if (n_split < 1) {
				except.append("token identifier must have identifiable characters or use the special constant values \n");
			}
			else {
				for (string ident : identifiers) {
					int counter = 0;
					for (auto ident_2 : identifiers) {
						if (ident_2 == ident) {
							++counter;
						}
					}
					if (counter > 1) {
						except.append("identifier constants must be unique except  \n");
					}
				}
				bool mixed_up_identifier = (contains_ele(identifiers, string("IS_IDENTIFIER_ALPHA_NUM")) && contains_ele(identifiers, string("IS_IDENTIFIER_ALPHA_LOWER"))) ||
					(contains_ele(identifiers, string("IS_IDENTIFIER_ALPHA_NUM")) && contains_ele(identifiers, string("IS_IDENTIFIER_ALPHA_UPPER")));
				if (mixed_up_identifier) {
					except.append("identifier reserved must not use 'IS_IDENTIFIER_ALPHA_NUM' with the others \n ");
				}
			}

		}
		return initial_content == except;
	}

	//@get_filtered_token utility function to get a list of filtered token by key value @value
	//
	//@tokens a list of input tokens
	//@value a key used in filtering for the list of tokens
	vector<Token> get_filtered_token(const vector<Token> tokens, DIAL_LEXER_VALUE value) {
		vector<Token> vec;
		std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(vec), [&](Token token) {return token.get_value() == value; });
		return vec;
	}

	//@trim utility function for a trimmed representation of an input string
	//
	//@val input string to be trimmed
	string trim(string val) {
		std::regex reg("^\\s+|\\s+$");
		return regex_replace(val, reg, "");
	}

	//@verify_regex_tokens_integrity utility function to validate a list of input token for regex lexer type
	//
	//@input_tokens a list of input tokens
	//@except a string containing exception message
	bool verify_regex_tokens_integrity(vector<Token>& input_tokens, string& except) {
		string initial_content = string(except);
		for (auto token : input_tokens) {
			int counter = 0;
			for (auto token_2 : input_tokens) {
				if (token.get_lexeme() == token_2.get_lexeme()) {
					++counter;
				}
			}
			if (counter > 1 && !(token.get_value() == DIAL_LEXER_VALUE::DIAL_NONE)) {
				except.append("regex tokens must be unique  -> " + token.get_lexeme() + "\n");
			}
		}
		return initial_content == except;
	}

	//@get_matched_token utility function to retrieve a token from a list @input_tokens based on a regex matcher @matcher
	//
	//@input_tokens a list of input tokens
	//@matcher an instance of std::regex matcher
	Token get_matched_token(std::smatch& matcher, vector<Token>& input_tokens) {
		for (size_t i = 1; i < matcher.size(); i++) {
			string content = trim(matcher.str(i));
			if (!content.empty()) {
				return input_tokens.at(i - 1);
			}

		}
		return Token();
	}

	//@get_error_token utility function to get error token value
	//@val input string to be tested for error
	string get_error_token(const string& val) {
		std::string error_token;
		error_token.reserve(val.size());
		std::remove_copy_if(
			begin(val), end(val),
			std::back_inserter(error_token),
			[l = std::locale{}](auto ch) { return std::isspace(ch, l); }
		);
		return error_token;
	}

	//@DialLexer Lexer class for validating and  splitting input based on predefined token rules
	//
	class DialLexer {
	public:
		//@add_token method to add a token to a  list @input_tokens
		//
		//@token token to be added to @input_tokens 
		void add_token(Token token)
		{
			this->input_tokens.push_back(token);
		}

		//@split method to split a source content @raw based on the lexer type
		//
		//@raw source content to be splitted 
		vector<Token> split(string raw)
		{
			reset_state();
			this->source = raw;
			vector<Token> tokens = this->type == LexerType::RAW ? raw_splitter() : regex_splitter();
			return tokens;
		}

		//@Lexer-constructor takes in a lexer type defaulted at @raw
		//
		//@type type of lexer to be used for splitting
		DialLexer(LexerType type = LexerType::RAW) : type(type)
		{
		}

		//@input_token_size get the size of input token list
		//
		size_t input_token_size()
		{
			return input_tokens.size();
		}
		
		//@set_comment set the value of @comment_begin and @comment_end
		//
		//@begin begining of comment token
		//@end end of comment token
		void set_comment(string begin, string end = "\n") {
			this->comment_begin = begin;
			this->comment_end = end;
		}
	private:
		LexerType type;
		string source;
		int current = 0, line = 1;
		string comment_begin = "", comment_end = "";

		//@advance move by one char in @source content
		//
		char advance()
		{
			++current;
			return current < get_eof() ? this->source[current] : -1;
		}

		//@advance operation function on @source content with line aware attribute
		//
		//@func a function implementing @source operation 
		char advance_with_line(std::function<char(void)> func)
		{
			char c = func();
			if (c == '\n') {
				line++;
			}
			return c;
		}

		//@peek look ahead at position 1 from @source
		//
		char peek()
		{
			return peek_lookahead(1);
		}
		bool has_error = false;

		//@match match the current character from @source with an expected character
		//
		//@expected a function implementing @source operation 
		bool match(char expected)
		{
			return current < get_eof() ? this->source[current] == expected : false;
		}
		//@match_comment match the current character from @source with a comment start or end token
		//
		//@handle_start a condition representing matching start or end comment
		bool match_comment(bool handle_start) {
			vector<char> c_container_;
			string lex = handle_start ? this->comment_begin : this->comment_end;
			copy(lex.begin(), lex.end(), back_inserter(c_container_));
			int size_ = c_container_.size();
			bool matched = false;
			int inner_count = 0;
			for (int i = 0; i < size_; i++) {
				if (!is_eof() && (peek_lookahead(i) == c_container_.at(i))) {
					inner_count++;
				}
				else if (is_eof() && !handle_start) {
					has_error = true;
					return false;
				}
				else {
					return false;
				}
			}
			matched = inner_count == size_;
			return matched;
		}

		//@match_word match the current characters from @source with  expected characters
		//
		//@word word to be matched against
		bool match_word(string word) {
			vector<char> c_container_;
			copy(word.begin(), word.end(), back_inserter(c_container_));
			int size_ = c_container_.size();
			int inner_count = 0;
			for (int i = 0; i < size_; i++) {
				if (!is_eof() && (peek_lookahead(i) == c_container_.at(i))) {
					inner_count++;
				}
				else {
					return false;
				}
			}
			return inner_count == size_;
		}

		//@match_identifiers match the identifiers from validated @unknown_identifiers
		//
		//@identifier_cont container for identifier
		//@identifier current identifier to be matched
		//@watcher replica of @identifier
		//@start value to determine if it is the start of matching
		void match_identifiers(vector<string>& identifier_cont, string& identifier, string& watcher, bool start) {
			std::for_each(identifier_cont.begin(), identifier_cont.end(), [&](string& lexeme) {
				if (lexeme == dial::IS_IDENTIFIER_ALPHA_NUM) {
					string content = match_get_alnum_ident();
					if (!content.empty()) {
						std::for_each(content.begin(), content.end(), [&](char& ele) { advance(); });
						identifier.append(content);
					}
				}
				if (lexeme == dial::IS_IDENTIFIER_ALPHA_LOWER) {
					string content = match_get_alpha_lower_ident();
					if (!content.empty()) {
						std::for_each(content.begin(), content.end(), [&](char& ele) { advance(); });
						identifier.append(content);
					}
				}
				if (lexeme == dial::IS_IDENTIFIER_ALPHA_UPPER) {
					string content = match_get_alpha_upper_ident();
					if (!content.empty()) {
						std::for_each(content.begin(), content.end(), [&](char& ele) { advance(); });
						identifier.append(content);
					}
				}
				if (match_word(lexeme)) {
					std::for_each(lexeme.begin(), lexeme.end(), [&](char& ele) { advance(); });
					identifier.append(lexeme);
				}
				});
			if (watcher != identifier) {
				watcher = identifier;
				match_identifiers(identifier_cont, identifier, watcher, false);
			}
			else if (start) {
				match_identifiers(identifier_cont, identifier, watcher, false);
			}
		}

		//@reset_state reset @input_tokens, @current, @line state
		//
		void reset_state()
		{
			output_tokens.clear();
			current = 0;
			line = 1;
		}

		//@match_get_double match double value starting from current character
		//
		string match_get_double() {
			string appended_double = "", current_lex = "";
			for (int i = 0; i < get_eof(); i++) {
				char prob_double = peek_lookahead(i);
				current_lex.push_back(prob_double);
				bool is_double = validate_double(current_lex);
				if (is_double) {
					appended_double = current_lex;
					continue;
				}
				else {
					break;
				}
			}
			return appended_double;
		}

		//@match_get_alnum_ident match alphanumeric identifiers from validated @unknown_identifiers
		//
		string match_get_alnum_ident() {
			string appended_identifier = "";
			for (int i = 0; i < get_eof(); i++) {
				char prob_ident = peek_lookahead(i);
				if (isalnum(prob_ident)) {
					appended_identifier.push_back(prob_ident);
				}
				else {
					break;
				}
			}

			return appended_identifier;
		}

		//@match_get_alpha_lower_ident match lower case alphabet identifiers from validated @unknown_identifiers
		//
		string match_get_alpha_lower_ident() {
			string appended_identifier = "";
			for (int i = 0; i < get_eof(); i++) {
				char prob_ident = peek_lookahead(i);
				if (isalpha(prob_ident) && islower(prob_ident)) {
					appended_identifier.push_back(prob_ident);
				}
				else {
					break;
				}
			}

			return appended_identifier;
		}

		//@match_get_alpha_upper_ident match upper case alphabet identifiers from validated @unknown_identifiers
		//
		string match_get_alpha_upper_ident() {
			string appended_identifier = "";
			for (int i = 0; i < get_eof(); i++) {
				char prob_ident = peek_lookahead(i);
				if (isalpha(prob_ident) && isupper(prob_ident)) {
					appended_identifier.push_back(prob_ident);
				}
				else {
					break;
				}
			}

			return appended_identifier;
		}

		//@get_eof get @source size
		//
		int const get_eof()  const
		{
			return this->source.size();
		}

		//@is_eof determine if the current pointer is at end of file
		//
		bool const is_eof()
		{
			return current >= get_eof();
		}

		//@peek_lookahead look ahead to any given point from @source
		//
		char const peek_lookahead(int offset) const
		{
			int position = current + offset;
			return position < get_eof() ? this->source[position] : -1;
		}

		//@regex_splitter split @source content based on regex tokens
		//
		vector<Token> regex_splitter()
		{
			string exception_message = "";
			bool status_good = verify_regex_tokens_integrity(input_tokens, exception_message);
			if (!status_good) {
				has_error = true;
				throw DialLexerException(exception_message, line, current, true);
			}
			std::smatch matcher;
			string patterns = "";
			for (Token token : input_tokens) {
				patterns.append("(").append(token.get_lexeme()).append(")|");
			}
			string content = string(source);
			patterns.pop_back();
			std::regex reg(patterns, std::regex::extended);
			while (std::regex_search(content, matcher, reg))
			{
				int split_pos = matcher.position();
				current += (matcher.length() + split_pos);
				string rem = content.substr(0, split_pos);

				Token matched_token = get_matched_token(matcher, input_tokens);
				if (matched_token == Token()) {
					has_error = true;
					lex_exception.add_info("can't match whitespaces", line, current, false);
				}
				std::string error_token = get_error_token(rem);
				if (!error_token.empty()) {
					has_error = true;
					lex_exception.add_info(rem, line, current, false);
				}
				for (auto t : rem) {
					if (t == '\n') {
						++line;
					}
				}
				Token token_ = Token(matched_token.get_type(), matcher.str(), matched_token.get_value());
				token_.set_line(line);
				output_tokens.push_back(token_);
				content = content.substr(split_pos + matcher.length());
			}
			std::string error_token = get_error_token(content);
			if (!error_token.empty()) {
				has_error = true;
				lex_exception.add_info(content, line, current, false);
			}
			if (has_error) {
				throw lex_exception;
			}
			return output_tokens;
		}

		//@raw_splitter split @source content based on raw hand crafted tokenizer
		//
		vector<Token> raw_splitter()
		{
			string exception_message = "";
			vector<Token> vec = std::move(get_filtered_token(this->input_tokens, DIAL_LEXER_VALUE::DIAL_STRING_END));
			Token end_token = !vec.empty() ? vec.at(0) : Token{};

			bool status_good = verify_raw_tokens_integrity(input_tokens, exception_message, unknown_identifiers, comment_begin, comment_end);
			if (!status_good) {
				has_error = true;
				throw DialLexerException(exception_message, line, current, true);
			}

			while (current < get_eof()) {
				int previous_counter = current;
				switch (peek_lookahead(0)) {
				case ' ': {
					advance();
					while (match(' ')) {
						advance();
					}
					break;
				}
				case '\n': {
					line++;
					advance();
				}
						 break;
				}
				if (comment_begin != "") {
					if (match_comment(true)) {
						while (!is_eof() && !match_comment(false)) {
							char c = advance();
							if (c == '\n') {
								line++;
							}
						}
						//pull out of comment token
						if (!is_eof()) {
							for (size_t i = 0; i < comment_end.size(); i++) {
								advance();
							}
						}
					}
				}
				for (vector<Token>::iterator token_iter = input_tokens.begin(); token_iter < input_tokens.end(); token_iter++) {
					Token token = *token_iter;
					string start_lex = token.get_lexeme();
					size_t token_len = start_lex.size();
					//match keywords
					//handle string offset
					bool is_string = static_cast<int>(token.get_value()) == static_cast<int>(DIAL_LEXER_VALUE::DIAL_STRING_START) ||
						static_cast<int>(token.get_value()) == static_cast<int>(DIAL_LEXER_VALUE::DIAL_STRING_END);
					if (!is_string) {
						bool matched_all = token_len > 0 ? true : false;
						for (size_t i = 0; i < token_len; i++) {
							char ff_char = peek_lookahead(i);
							if (ff_char != start_lex[i]) {
								matched_all = false;
								break;
							}
						}
						if (matched_all) {
							for (size_t i = 0; i < token_len; i++) {
								advance();
							}
							Token matched_token = token;
							matched_token.set_line(line);
							output_tokens.push_back(matched_token);
							break;
						}
					}
					switch (token.get_value()) {
					case DIAL_LEXER_VALUE::DIAL_STRING_START: {
						string word;
						if (match_word(start_lex)) {
							char c_f = advance();
							word.push_back(c_f);
							while (!is_eof() && !match_word(end_token.get_lexeme())) {
								char c = peek();
								if (c == '\n') {
									line++;
								}
								word.push_back(c);
								advance();
							}
							//pull out of the last string token
							advance();
							Token token_ = Token(token.get_type(), start_lex + word, DIAL_LEXER_VALUE::DIAL_STRING);
							token_.set_line(line);
							output_tokens.push_back(token_);
						}
					}
					case DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE: {
						string double_content = match_get_double();
						if (!double_content.empty()) {
							std::for_each(double_content.begin(), double_content.end(), [&](char& ele) { advance(); });
							Token token_ = Token(get_type(input_tokens, DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE), double_content, DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE);
							token_.set_line(line);
							output_tokens.push_back(token_);
						}
					}
					case DIAL_LEXER_VALUE::DIAL_IDENTIFIER: {
						string identifier = "", watcher = "";
						match_identifiers(unknown_identifiers, identifier, watcher, true);
						if (!identifier.empty()) {
							Token token_ = Token(get_type(input_tokens, DIAL_LEXER_VALUE::DIAL_IDENTIFIER), identifier, DIAL_LEXER_VALUE::DIAL_IDENTIFIER);
							token_.set_line(line);
							output_tokens.push_back(token_);
							identifier.clear();
						}
					}
					}
				}
				if (previous_counter == current) {
					//handle error
					has_error = true;

					//already at new line
					if (match('\n')) {
						exception_message.push_back('\n');
						advance();
						continue;
					}
					string error_token = { peek_lookahead(0) };
					char a = ' ';
					while (((a = advance()), a != '\n') && !is_eof()) {
						error_token.push_back(a);
					}
					lex_exception.add_info(error_token, line, current, false);
					line++;//using new line or eof as breakpoint
					advance();
				}
			}
			if (has_error) {
				throw lex_exception;
			}
			return output_tokens;
		}
		vector<Token> input_tokens;
		vector<Token> output_tokens;
		vector<string> unknown_identifiers;
	};

	
}//end namespace dial
#endif // end lexer lib