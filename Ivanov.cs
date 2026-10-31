using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace System
{
    internal class Program
    {
        const int WIDTH = 20;
        const int HEIGHT = 10;

        [StructLayout(LayoutKind.Sequential)]
        public struct COORD
        {
            public short X;
            public short Y;

            public COORD(short x, short y)
            {
                X = x;
                Y = y;
            }
        }

        [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode)]
        public struct CHAR_INFO
        {
            [FieldOffset(0)]
            public char UnicodeChar;

            [FieldOffset(2)]
            public short Attributes;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SMALL_RECT
        {
            public short Left;
            public short Top;
            public short Right;
            public short Bottom;
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr GetStdHandle(int nStdHandle);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool WriteConsoleOutputW(
            IntPtr hConsoleOutput,
            [In] CHAR_INFO[] lpBuffer,
            COORD dwBufferSize,
            COORD dwBufferCoord,
            ref SMALL_RECT lpWriteRegion);

        const int STD_OUTPUT_HANDLE = -11;
        const short FOREGROUND_GREEN = 0x0002;
        const short FOREGROUND_INTENSITY = 0x0008;

        static void Main()
        {
            IntPtr hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            CHAR_INFO[] buffer = new CHAR_INFO[WIDTH * HEIGHT];

            // Заполняем буфер пробелами
            for (int i = 0; i < buffer.Length; i++)
            {
                buffer[i].UnicodeChar = ' ';
                buffer[i].Attributes = 7;
            }

            COORD bufferSize = new COORD(WIDTH, HEIGHT);
            COORD bufferCoord = new COORD(0, 0);

            SMALL_RECT writeRegion = new SMALL_RECT
            {
                Left = 0,
                Top = 0,
                Right = WIDTH - 1,
                Bottom = HEIGHT - 1
            };

            Console.Clear();

            // Заполнение треугольника снизу вверх
            for (int y = HEIGHT - 1; y >= 0; y--)
            {
                int count = 2 * (y + 1);
                int startX = (WIDTH - count) / 2;

                for (int x = startX; x < startX + count; x++)
                {
                    int index = y * WIDTH + x;

                    buffer[index].UnicodeChar = '#';
                    buffer[index].Attributes =
                        FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                }

                bool success = WriteConsoleOutputW(
                    hConsole,
                    buffer,
                    bufferSize,
                    bufferCoord,
                    ref writeRegion);

                if (!success)
                {
                    Console.WriteLine(
                        "Ошибка вывода: " +
                        Marshal.GetLastWin32Error());
                    return;
                }

                Thread.Sleep(300);
            }

            Console.SetCursorPosition(0, HEIGHT + 1);
            Console.WriteLine("Нажмите Enter для выхода...");
            Console.ReadLine();
        }
    }
}
