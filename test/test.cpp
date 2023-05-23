#include "pch.h"
#include "parserphonepad/parserphonepad.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace iron
{
	TEST_CLASS(parserphonepad)
	{
	private:
		ParserPhonePad parser = ParserPhonePad();

	public:

		/// <summary>
		/// Test a set of known inputs against their known outputs, as given via requirements.
		/// </summary>
		TEST_METHOD(TestKnownInputs)
		{
			const vector<tuple<const char*, const char*>> inputs = vector{
				tuple { "33#", "E" },
				tuple { "227*#", "B" },
				tuple { "4433555 555666#", "HELLO" },
			};

			for (auto i = inputs.cbegin(); i != inputs.cend(); ++i) {
				string input;
				string expected;
				tie(input, expected) = *i;

				auto result = parser.parse(input);
				Assert::IsTrue(result.ok());
				Assert::AreEqual(expected, result.result());
			}
		}

		/// <summary>
		/// Input consisting only of the termination character should produce an empty result.
		/// </summary>
		TEST_METHOD(TestLoneTerminator)
		{
			auto result = parser.parse("#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string(""), result.result());
		}

		/// <summary>
		/// Input consisting only of deletes and the termination character should produce an empty result.
		/// </summary>
		TEST_METHOD(TestLoneDeletes)
		{
			auto result = parser.parse("****#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string(""), result.result());
		}

		/// <summary>
		/// Multiple '*' characters should not cause an issue, and deleting beyond the start of the result string should
		/// also not cause an issue.
		/// </summary>
		TEST_METHOD(TestMultipleDeletes)
		{
			auto result = parser.parse("2 **** 3#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string("D"), result.result());
		}

		/// <summary>
		/// Attempts to parse an empty string should return an informative error.
		/// </summary>
		TEST_METHOD(TestEmpty)
		{
			auto result = parser.parse("");
			Assert::IsFalse(result.ok());
			Assert::AreNotEqual(result.error.find("empty"), string::npos);
		}

		/// <summary>
		/// If any input exists beyond the terminator character, then return an informative error.
		/// </summary>
		TEST_METHOD(TestExtraInput)
		{
			auto result = parser.parse("123# 123#");
			Assert::IsFalse(result.ok());
			Assert::AreNotEqual(result.error.find("input beyond"), string::npos);
		}

		/// <summary>
		/// An informative error should be produced if the input doesn't end with the terminator character.
		/// </summary>
		TEST_METHOD(TestNoTerminator)
		{
			auto result = parser.parse("123");
			Assert::IsFalse(result.ok());
			Assert::AreNotEqual(result.error.find("terminating"), string::npos);
		}

		/// <summary>
		/// The '*' character, arriving in the middle of a multiple press sequence, should cancel that sequence.
		/// Otherwise, it should delete the most recently-added character in the output.
		/// </summary>
		TEST_METHOD(TestDelete)
		{
			auto result = parser.parse("22*34*#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string("D"), result.result());
		}

		/// <summary>
		/// Multiple presses of a button should advance among the characters assigned to that button.
		/// 
		/// Additional presses beyond the number of characters assigned to the button should 'wrap around' to the beginning
		/// of the character set.
		/// 
		/// '0' character should insert a space.
		/// ' ' character should 'wait'.
		/// </summary>
		TEST_METHOD(TestSpaceAndSequence)
		{
			auto result = parser.parse("318884430666 6662222#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string("D&VHD OOA"), result.result());
		}

		/// <summary>
		/// Multiple 'wait' characters should effectively behave as a single wait character.
		/// Wait characters at the start of input should not cause an error.
		/// </summary>
		TEST_METHOD(TestMultipleWait)
		{
			auto result = parser.parse("   11  23#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string("'AD"), result.result());
		}

		/// <summary>
		/// A larger input combining aspects of previous tests.
		/// </summary>
		TEST_METHOD(TestCombined)
		{
			auto result = parser.parse("32888444301116 6 666 66611110666 66 * 55 51 * *#");
			Assert::IsTrue(result.ok());
			Assert::AreEqual(string("DAVID (MMOO) OK"), result.result());
		}
	};
}
