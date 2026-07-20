// Test: Type system (cn_type_*)
#include "../cnotes.h"

int main(void) {
    // Test cn_type_equals - primitive types
    {
        CN_ASSERT(cn_type_equals(&CN_TYPE_INT, &CN_TYPE_INT) == true);
        CN_ASSERT(cn_type_equals(&CN_TYPE_CHAR, &CN_TYPE_CHAR) == true);
        CN_ASSERT(cn_type_equals(&CN_TYPE_FLOAT, &CN_TYPE_FLOAT) == true);
        CN_ASSERT(cn_type_equals(&CN_TYPE_VOID, &CN_TYPE_VOID) == true);

        CN_ASSERT(cn_type_equals(&CN_TYPE_INT, &CN_TYPE_CHAR) == false);
        CN_ASSERT(cn_type_equals(&CN_TYPE_INT, &CN_TYPE_FLOAT) == false);
        CN_ASSERT(cn_type_equals(&CN_TYPE_INT, &CN_TYPE_VOID) == false);
    }

    // Test cn_type_make_pointer
    {
        Cn_Type ptr_int = cn_type_make_pointer(&CN_TYPE_INT);
        CN_ASSERT(ptr_int.kind == CN_POINTER);
        CN_ASSERT(cn_type_equals(ptr_int.pointer.ptr_to, &CN_TYPE_INT));

        Cn_Type ptr_char = cn_type_make_pointer(&CN_TYPE_CHAR);
        CN_ASSERT(ptr_char.kind == CN_POINTER);
        CN_ASSERT(cn_type_equals(ptr_char.pointer.ptr_to, &CN_TYPE_CHAR));

        // Pointer to pointer
        Cn_Type ptr_ptr_int = cn_type_make_pointer(&ptr_int);
        CN_ASSERT(ptr_ptr_int.kind == CN_POINTER);
        CN_ASSERT(ptr_ptr_int.pointer.ptr_to->kind == CN_POINTER);
    }

    // Test cn_type_make_qualified
    {
        Cn_Type const_int = cn_type_make_qualified(CN_TYPE_QUALIFIED_CONSTANT, &CN_TYPE_INT);
        CN_ASSERT(const_int.kind == CN_QUALIFIED);
        CN_ASSERT(const_int.flags & CN_TYPE_QUALIFIED_CONSTANT);

        Cn_Type volatile_int = cn_type_make_qualified(CN_TYPE_QUALIFIED_VOLATILE, &CN_TYPE_INT);
        CN_ASSERT(volatile_int.kind == CN_QUALIFIED);
        CN_ASSERT(volatile_int.flags & CN_TYPE_QUALIFIED_VOLATILE);
    }

    // Test cn_type_is_constant
    {
        Cn_Type const_int = cn_type_make_qualified(CN_TYPE_QUALIFIED_CONSTANT, &CN_TYPE_INT);
        CN_ASSERT(cn_type_is_constant(&const_int) == true);
        CN_ASSERT(cn_type_is_constant(&CN_TYPE_INT) == false);
    }

    // Test cn_type_unqualified
    {
        Cn_Type const_int = cn_type_make_qualified(CN_TYPE_QUALIFIED_CONSTANT, &CN_TYPE_INT);
        Cn_Type *unqual = cn_type_unqualified(&const_int);
        CN_ASSERT(cn_type_equals(unqual, &CN_TYPE_INT));

        // Already unqualified
        Cn_Type *unqual2 = cn_type_unqualified(&CN_TYPE_INT);
        CN_ASSERT(cn_type_equals(unqual2, &CN_TYPE_INT));
    }

    // Test cn_type_is_arithmetic
    {
        CN_ASSERT(cn_type_is_arithmetic(&CN_TYPE_INT) == true);
        CN_ASSERT(cn_type_is_arithmetic(&CN_TYPE_CHAR) == true);
        CN_ASSERT(cn_type_is_arithmetic(&CN_TYPE_FLOAT) == true);
        CN_ASSERT(cn_type_is_arithmetic(&CN_TYPE_VOID) == false);

        Cn_Type ptr_int = cn_type_make_pointer(&CN_TYPE_INT);
        CN_ASSERT(cn_type_is_arithmetic(&ptr_int) == false);
    }

    // Test cn_type_is_scalar
    {
        CN_ASSERT(cn_type_is_scalar((Cn_Type *)&CN_TYPE_INT) == true);
        CN_ASSERT(cn_type_is_scalar((Cn_Type *)&CN_TYPE_CHAR) == true);
        CN_ASSERT(cn_type_is_scalar((Cn_Type *)&CN_TYPE_FLOAT) == true);

        Cn_Type ptr_int = cn_type_make_pointer(&CN_TYPE_INT);
        CN_ASSERT(cn_type_is_scalar(&ptr_int) == true);  // Pointers are scalar

        CN_ASSERT(cn_type_is_scalar((Cn_Type *)&CN_TYPE_VOID) == false);
    }

    // Test cn_type_hash - consistency
    {
        uint64_t h1 = cn_type_hash(&CN_TYPE_INT);
        uint64_t h2 = cn_type_hash(&CN_TYPE_INT);
        CN_ASSERT(h1 == h2);

        uint64_t h3 = cn_type_hash(&CN_TYPE_CHAR);
        CN_ASSERT(h1 != h3);

        Cn_Type ptr_int = cn_type_make_pointer(&CN_TYPE_INT);
        uint64_t h4 = cn_type_hash(&ptr_int);
        CN_ASSERT(h4 != h1);
    }

    // Test cn_type_is_compatible
    {
        CN_ASSERT(cn_type_is_compatible(&CN_TYPE_INT, &CN_TYPE_INT) == true);
        CN_ASSERT(cn_type_is_compatible(&CN_TYPE_INT, &CN_TYPE_CHAR) == false);
        CN_ASSERT(cn_type_is_compatible(&CN_TYPE_FLOAT, &CN_TYPE_FLOAT) == true);
        CN_ASSERT(cn_type_is_compatible(&CN_TYPE_INT, &CN_TYPE_FLOAT) == false);
    }

    // Test cn_type_greatest_arithmetic_rank
    {
        Cn_Type *result;

        // float > int
        result = cn_type_greatest_arithmetic_rank(&CN_TYPE_FLOAT, &CN_TYPE_INT);
        CN_ASSERT(cn_type_equals(result, &CN_TYPE_FLOAT));

        // int > char (typically)
        result = cn_type_greatest_arithmetic_rank(&CN_TYPE_INT, &CN_TYPE_CHAR);
        CN_ASSERT(cn_type_equals(result, &CN_TYPE_INT));
    }

    return 0;
}
