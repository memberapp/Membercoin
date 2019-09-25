import contextlib
import re
import os

class TestNode():
    def __init__(self, rpc, datadir):
        self.rpc = rpc
        self.datadir = datadir

    def __getattr__(self, name):
        """
        Assume anything not implemented here is an rpc call and pass it on
        """
        return getattr(self.rpc, name)

    def _raise_assertion_error(self, error):
        assert False, error

    @contextlib.contextmanager
    def assert_debug_log(self, *, expected_msgs, unexpected_msgs):
        debug_log = os.path.join(self.datadir, 'regtest', 'debug.log')
        with open(debug_log, encoding='utf-8') as dl:
            dl.seek(0, 2)
            prev_size = dl.tell()
        try:
            yield
        finally:
            with open(debug_log, encoding='utf-8') as dl:
                dl.seek(prev_size)
                log = dl.read()
            print_log = " - " + "\n - ".join(log.splitlines())
            for expected_msg in expected_msgs:
                if re.search(re.escape(expected_msg), log, flags=re.MULTILINE) is None:
                    self._raise_assertion_error(
                        'Expected message "{}" does not partially match log:\n\n{}\n\n'.format(expected_msg, print_log))

            for unexpected in unexpected_msgs:
                if re.search(re.escape(unexpected), log, flags=re.MULTILINE) is not None:
                    self._raise_assertion_error(
                        'Unexpected message "{}" matched log:\n\n{}\n\n'.format(unexpected, print_log))
