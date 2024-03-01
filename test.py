import os
import unittest
import subprocess


class LavaSH(unittest.TestCase):
    res_score = 0

    def _run(self, command, expected_out='', expected_err='', code=0, pre=None, post=None):
        if pre:
            pre(self)
        p = subprocess.Popen(['./lavash', '-c', command], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        self.assertEqual(code, p.returncode)
        self.assertEqual(out.decode(), expected_out)
        self.assertEqual(err.decode(), expected_err)
        if post:
            post(self)

    @classmethod
    def tearDownClass(cls):
        print()
        print('Score:', cls.res_score)

    @staticmethod
    def _remove_file(name):
        def inner(self):
            if os.path.exists(name):
                os.remove(name)
        return inner

    @staticmethod
    def _create_file(name, data):
        def inner(self):
            if os.path.exists(name):
                os.remove(name)
            with open(name, 'w') as f:
                f.write(data)
        return inner

    @staticmethod
    def _check_file_contains(name, expected):
        def inner(self):
            with open(name, 'r') as f:
                self.assertEqual(f.read(), expected)
        return inner

    @staticmethod
    def score(score):
        def decorator(func):
            def inner(self, *args, **kwargs):
                try:
                    res = func(self, *args, **kwargs)
                except Exception as e:
                    raise
                else:
                    LavaSH.res_score += score
                    return res
            return inner
        return decorator

    @score(10)
    def test_simple(self):
        self._run('echo', '\n')
        self._run('echo hello', 'hello\n')
        self._run('echo hello world', 'hello world\n')

    @score(10)
    def test_relative_path(self):
        self._run('./tools/print_args', './tools/print_args\n')
        self._run('./tools/print_args 1', './tools/print_args\n1\n')
        self._run('./tools/print_args "1"', './tools/print_args\n1\n')
        self._run('./tools/print_args 1 "2 3" 4', './tools/print_args\n1\n2 3\n4\n')

    @score(10)
    def test_pipe(self):
        self._run('echo hello | wc', '      1       1       6\n')

    @score(10)
    def test_out_files(self):
        self._run('echo hello > output.txt',
                  pre=self._remove_file('output.txt'),
                  post=self._check_file_contains('output.txt', 'hello\n'))
        self._run('echo hello >output.txt',
                  pre=self._remove_file('output.txt'),
                  post=self._check_file_contains('output.txt', 'hello\n'))

    @score(10)
    def test_in_files(self):
        self._run('wc < input.txt',
                  ' 9  9 26\n',
                  pre=self._create_file('input.txt', 'abracabra\n1\n2\n3\n4\n5\n6\n7\n8\n'))


if __name__ == '__main__':
    unittest.main()
