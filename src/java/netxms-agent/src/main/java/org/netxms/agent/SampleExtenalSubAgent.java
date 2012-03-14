package org.netxms.agent;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class SampleExtenalSubAgent implements ParameterProvider, ListProvider {
    public static void main(final String[] args) throws IOException {
        new SampleExtenalSubAgent().start();
    }

    private void start() throws IOException {
        final PipeConnector connector = new PipeConnector("java");
        final Engine engine = new Engine(connector);
        engine.registerParameterProvider(this);
        engine.registerListParameterProvider(this);
        engine.start();

        System.out.println("Started, press <Enter> to stop");

        //noinspection ResultOfMethodCallIgnored
        System.in.read();

        System.out.println("Stopping");

        engine.stop();
    }

    @Override
    public List<Parameter> getLists() {
        return new ArrayList<Parameter>(0);
    }

    @Override
    public List<Parameter> getParameters() {
        final List<Parameter> parameters = new ArrayList<Parameter>(2);
        parameters.add(new xParameter("Java.Parameter1", "Description 1", ParameterType.INT) {
            @Override
            public String getValue(final String argument) {
                return "123";
            }
        });
        parameters.add(new xParameter("Java.Parameter2", "Description 2", ParameterType.STRING) {
            @Override
            public String getValue(final String argument) {
                return "some string";
            }
        });
        parameters.add(new ListParameter("Java.List", "Sample List", ParameterType.STRING) {
            @Override
            public String[] getListValue(final String argument) {
                return new String[]{"line1", "line2", "line3"};
            }
        });
        return parameters;
    }
}
