#include "Test.h"

TEST_CASE("Covergroup data type") {
    auto tree = SyntaxTree::fromText(R"(
module m;
    covergroup cg; endgroup

    cg c1 = null;
    cg c2 = c1;
    cg c3 = new;

    initial begin
        if (c1 == c2 || c1 == null || c1 !== null || c2 === c1) begin
        end

        if (c1) begin
            c2 = c1 ? c1 : null;
        end
    end

    int arr[cg];
    initial begin
        arr[c1] = 1;
        arr[c2] = 2;
        arr[null] = 3;
    end
endmodule
)");

    Compilation compilation;
    compilation.addSyntaxTree(tree);
    NO_COMPILATION_ERRORS;
}

TEST_CASE("Covergroup with arguments") {
    auto tree = SyntaxTree::fromText(R"(
module m;
    int foo;
    covergroup cg(input int a, ref int b = foo, input int c = 1); endgroup

    cg c1 = new(3);
    cg c2 = new(3, foo, 52);
endmodule
)");

    Compilation compilation;
    compilation.addSyntaxTree(tree);
    NO_COMPILATION_ERRORS;
}

TEST_CASE("Covergroup basic errors") {
    auto tree = SyntaxTree::fromText(R"(
module m;
    covergroup cg(int a, output b); endgroup

    logic foo;
    cg c1 = new(3, foo);

    localparam cg c2 = new(3, foo);
    localparam int p = baz();

    function automatic int baz;
        cg c3;
        cg c4 = c3;
    endfunction
endmodule
)");

    Compilation compilation;
    compilation.addSyntaxTree(tree);

    auto& diags = compilation.getAllDiagnostics();
    REQUIRE(diags.size() == 3);
    CHECK(diags[0].code == diag::CovergroupOutArg);
    CHECK(diags[1].code == diag::ConstEvalCovergroupType);
    CHECK(diags[2].code == diag::ConstEvalCovergroupType);
}