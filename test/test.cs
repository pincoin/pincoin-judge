using System;

public class Program {
    public static void Main() {
        Console.WriteLine("Hello, world C#");

        string s = Console.ReadLine();
        string[] ss = s.Split();
        int a = int.Parse(ss[0]);
        int b = int.Parse(ss[1]);
        Console.WriteLine(a + b);
    }
}
