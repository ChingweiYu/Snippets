# Given a positive integer, 
# return its corresponding column title as appear in an Excel sheet.
# Time Complexity: O(n)
# Space Complexity: O(1)

class Solution:
     # @param {integer} n
     # @return {string}
     def convertToTitle(self, n):
        result = []
        while n > 0:
            result.insert(0, chr(ord('A') + (n - 1) % 26))
            n = (n - 1) / 26
        return ''.join(result)

if __name__ == "__main__":
    solution = Solution()
    print solution.convertToTitle(26)
