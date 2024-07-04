import re
import unittest
import tictactoe as ttt
import subprocess

class TicTacToeTests(unittest.TestCase):


    def testTicTacToeBuildAndReset(self):
        t = ttt.TicTacToe(3)
        self.assertEquals(t.board, [0,0,0,0,0,0,0,0,0])
        self.assertEquals(t.turn, 1)
        state = [0,0,0,1,0,0,2,0,0]
        t.reset(state)
        self.assertEquals(t.board, state)
        self.assertEquals(t.turn, 1)
        state = [0,0,1,1,0,0,2,0,0]
        t.reset(state)
        self.assertEquals(t.board, state)
        self.assertEquals(t.turn, 2)

    def testMove(self):
        t = ttt.TicTacToe(3)
        self.assertEquals(t.board, [0,0,0,0,0,0,0,0,0])
        t.move(0)
        self.assertEquals(t.board, [1,0,0,0,0,0,0,0,0])
        t.move(1)
        self.assertEquals(t.board, [1,2,0,0,0,0,0,0,0])
        state = [0,0,0,1,0,0,2,0,0]
        t.reset(state)
        t.move(0)
        self.assertEquals(t.board, [1,0,0,1,0,0,2,0,0])

    def testIllegalMove(self):
        t = ttt.TicTacToe(3)
        self.assertEquals(t.board, [0,0,0,0,0,0,0,0,0])
        t.move(0)
        self.assertEquals(t.board, [1,0,0,0,0,0,0,0,0])
        r = t.move(0)
        self.assertFalse(r)
        self.assertEquals(t.board, [1,0,0,0,0,0,0,0,0])

    def testEndGame(self):
        t = ttt.TicTacToe(3)
        self.assertFalse(t.is_win())
        state = [1,1,1,0,2,2,0,0,0]
        t.reset(state)
        self.assertEquals(t.is_win(), (ttt.TicTacToe.Row, 0, 1))
        state = [2,1,0,1,1,1,0,2,2]
        t.reset(state)
        self.assertEquals(t.is_win(), (ttt.TicTacToe.Row, 3, 1))
        state = [2,0,0,2,1,1,2,0,1]
        t.reset(state)
        self.assertEquals(t.is_win(), (ttt.TicTacToe.Column, 0, 2))
        state = [2,0,0,1,2,1,0,1,2]
        t.reset(state)
        self.assertEquals(t.is_win(), (ttt.TicTacToe.DiagonalRight, 0, 2))
        state = [2,0,1,1,1,2,1,0,2]
        t.reset(state)
        self.assertEquals(t.is_win(), (ttt.TicTacToe.DiagonalLeft, 2, 1))
        state = [2,1,1,1,1,2,2,2,1]
        t.reset(state)
        self.assertEquals(t.is_win(), (ttt.TicTacToe.StaleMate, 0, 0))

    def testEndGameLargeBoard(self):
        t = ttt.TicTacToe(4)
        t.reset([1,1,1,0, 2,2,2,0, 0,0,0,0, 0,0,0,0])
        self.assertFalse(t.is_win())
        t.reset([1,1,1,1, 1,0,0,2, 2,0,0,2, 2,0,0,0])
        self.assertEquals(t.is_win(), (ttt.TicTacToe.Row, 0, 1))
        t.reset([1,1,1,2, 1,0,0,2, 0,0,0,2, 0,0,0,2])
        self.assertEquals(t.is_win(), (ttt.TicTacToe.Column, 3, 2))
