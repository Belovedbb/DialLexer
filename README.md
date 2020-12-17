<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]



<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/belovedbb/DialLexer">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Dial Lexer </h3>

  <p align="center">
    A Generic Regex/Hand Crafted  Language Tokenizer !
    <br />
    <a href="https://github.com/belovedbb/DialLexer/issues">Report Bug</a>
    ·
    <a href="https://github.com/belovedbb/DialLexer/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](/images/sample.gif)

There are many superb language lexer out there, but i wanted something simple and pluggable, the product of that is DialLexer.

Benefits:
* A lexer that can be plugged into a custom language for tokenization.
* Source code is easy to read and understand
* Easy to get started on -> Header only project
* :smile:

##### Details
  This project is divided into two lexer part 
  * Raw Lexer: This makes use of hand crafted lexer based on the TokenType Rule you will provide

  * Regex Lexer: This makes use of regex lexer based on the TokenType Rule you will provide

  Both methods can tokenize a given input and output a list of tokens matching the provided input. 
  
  If a lexer error occur during tokenization, a DialLexerException is thrown, this is also true for validation errors.

  This is a snippet of how to start out a simple lexing

  ```cpp
  #include "DialLexer.h"
  #include <cassert>

  enum class dial::TokenType : int {
    IF,
    ELSE
  };

  int main(){
      dial::DialLexer dial_lexer;

      //define syntax tokens
      dial_lexer.add_token({ dial::TokenType::IF, "if", dial::DIAL_LEXER_VALUE::DIAL_NONE });
      dial_lexer.add_token({ dial::TokenType::ELSE, "else", dial::DIAL_LEXER_VALUE::DIAL_NONE });

      std::string sc = "if \n else";
      //split the grammer into mapped tokens
      std::vector<dial::Token> splitted_tokens = dial_lexer.split(sc);

      assert(dial_lexer.input_token_size() == 2);
      assert(splitted_tokens.size() == 2);
  }
  ```

Of course,  this is not a perfect project and it's actually aimed at begininers. So I'll be adding more in the near future and constantly improving it. You may also suggest changes by forking this repo and creating a pull request or opening an issue!


### Built With

* [C++17](https://en.cppreference.com/w/cpp/17)
* [Doctest](https://github.com/onqtam/doctest)

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple example steps.

### Prerequisites

This is an example of how to run the test on your local machine.
* C++
  ```sh
  $ sudo g++ -o tester tester.cpp
  $ ./tester.out
  ```

<!-- USAGE EXAMPLES -->
## Usage

Documentation and Usage Examples coming soon ...

_[Documentation](https://example.com)_

<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/belovedbb/DialLexer/issues) for a list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

Contributions are **greatly appreciated**, please check the list below.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

Your Name - [@beloved_john](https://twitter.com/beloved_john) - belovedbb1@gmail.com

Project Link: [https://github.com/belovedbb/DialLexer](https://github.com/belovedbb/DialLexer)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [Crafting Interpreters](https://www.craftinginterpreters.com/)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/belovedbb/DialLexer.svg?style=for-the-badge
[contributors-url]: https://github.com/belovedbb/DialLexer/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/belovedbb/DialLexer.svg?style=for-the-badge
[forks-url]: https://github.com/belovedbb/DialLexer/network/members
[stars-shield]: https://img.shields.io/github/stars/belovedbb/DialLexer.svg?style=for-the-badge
[stars-url]: https://github.com/belovedbb/DialLexer/stargazers
[issues-shield]: https://img.shields.io/github/issues/belovedbb/DialLexer.svg?style=for-the-badge
[issues-url]: https://github.com/belovedbb/DialLexer/issues
[license-shield]: https://img.shields.io/github/license/belovedbb/DialLexer.svg?style=for-the-badge
[license-url]: https://github.com/belovedbb/DialLexer/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/ayooluwa-beloved-65710419a/
[product-screenshot]: images/sample.gif
