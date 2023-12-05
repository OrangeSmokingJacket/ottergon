﻿namespace Duckstax.EntityFramework.otterbrix.Extensions;

using Duckstax.EntityFramework.otterbrix.Infrastructure;
using Duckstax.EntityFramework.otterbrix.Infrastructure.Internal;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;

public static class SampleProviderDbContextOptionsBuilderExtensions
{
    public static DbContextOptionsBuilder UseotterbrixProvider(
        this DbContextOptionsBuilder optionsBuilder,
        Action<SampleProviderDbContextOptionsBuilder>? mySqlOptionsAction = null)
    {
        if (optionsBuilder is null)
            throw new ArgumentNullException(nameof(optionsBuilder));

        ((IDbContextOptionsBuilderInfrastructure)optionsBuilder).AddOrUpdateExtension(GetOrCreateExtension(optionsBuilder));
        ConfigureWarnings(optionsBuilder);

        mySqlOptionsAction?.Invoke(new SampleProviderDbContextOptionsBuilder(optionsBuilder));

        return optionsBuilder;
    }

    private static SampleProviderOptionsExtension GetOrCreateExtension(DbContextOptionsBuilder optionsBuilder)
        => optionsBuilder.Options.FindExtension<SampleProviderOptionsExtension>()
               ?? (SampleProviderOptionsExtension)new SampleProviderOptionsExtension().WithConnectionString("Filename=:memory:");

    private static void ConfigureWarnings(DbContextOptionsBuilder optionsBuilder)
    {
        var coreOptionsExtension = optionsBuilder.Options.FindExtension<CoreOptionsExtension>()
            ?? new CoreOptionsExtension();

        coreOptionsExtension = RelationalOptionsExtension.WithDefaultWarningConfiguration(coreOptionsExtension);

        ((IDbContextOptionsBuilderInfrastructure)optionsBuilder).AddOrUpdateExtension(coreOptionsExtension);
    }
}