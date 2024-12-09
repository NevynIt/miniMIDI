#pragma once

// #define AST_DEBUG 1

#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "decorator.h"
#include "d_functions.h"
#include "token.h"
#include "sequence.h"
#include "choice.h"
#include "repeat.h"
#include "regex.h"

//parser of objects of type O

namespace ast
{
    using namespace ast::_h; //helpers
    using namespace ast::_b; //bblocks
    using namespace ast::_d; //decorators
    using namespace ast::_t; //token
    using namespace ast::_s; //sequence
    using namespace ast::_c; //choice
    using namespace ast::_r; //repeat
    using namespace ast::_re; //regex
}
