namespace Duckstax.otterbrix.Tests;

using Duckstax.EntityFramework.otterbrix;

public class Tests
{
    private otterbrixWrapper wrapper { get; set; }

    [SetUp]
    public void Setup()
    {
        this.wrapper = new otterbrixWrapper();
    }

    [Test]
    public void Test1()
    {
        var result = this.wrapper.Execute();
        Assert.IsTrue(result);
    }
}