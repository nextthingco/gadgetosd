#!/usr/bin/env python
import unittest
import subprocess
import os
import shutil
import time

class TestGadget(unittest.TestCase):
    def setUp(self):
        self.server_env = os.environ.copy()
        self.server_env["GADGETOSD_SERVER"] = "127.0.0.1"
        self.server_env["GADGETOSD_PORT"] = "31415"
        self.server = subprocess.Popen( ["./gadgetosd"], env=self.server_env)

    def test_gadget_init(self):
        client_env = self.server_env.copy()
        self.client = subprocess.Popen( ["./gadget", "init", "testproject"], env=client_env)
        self.client.wait()
       
        self.assertIs(self.client.returncode, 0, "gadget init returned error code: "+ str(self.client.returncode) )
        self.assertTrue( os.path.isdir("testproject"), msg="Directory 'testproject' not created!")


    def tearDown(self):
        if os.path.isdir("testproject"):
            shutil.rmtree("testproject")

        if self.server.poll() == None:
            self.server.terminate()
            self.server.wait()

if __name__ == "__main__":
    unittest.main()
