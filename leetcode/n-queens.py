# Given an integer n, return all distinct solutions to the n-queens puzzle.
# Each solution contains a distinct board configuration of the n-queens' 
# placement, where 'Q' and '.' both indicate a queen and an empty space 
# respectively.

class Solution:
    # @param {integer} n
    # @return {string[][]}
    def solveNQueens(self, n):
        mx, d1, d2 = [False] * n, [False] * (2 * n), [False] * (2 * n)
        stack, solution, result = [(0, 0, False)], [0] * n, []
        while stack:
            y, x, flag = stack.pop()
            if y == n and flag == False:
                board = [["Q" if solution[i] == j else "." for j in xrange(n)]
                         for i in xrange(n)]
                result.append([''.join(item) for item in board])
            elif not flag:
                if x + 1 < n: stack.append((y, x + 1, False))
                if not mx[x] and not d1[x + y]  and not d2[n - 1 + x - y]:
                    mx[x] = d1[x + y] = d2[n - 1 + x - y] = True
                    solution[y] = x
                    stack.append((y, x, True))
                    stack.append((y + 1, 0, False))
            else:
                mx[x] = d1[x + y] = d2[n - 1 + x - y] = False
        return result

if __name__ == "__main__":
    solution = Solution()
    print solution.solveNQueens(4)
